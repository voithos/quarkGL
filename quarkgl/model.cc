#include <glad/glad.h>
#include <stb/stb_image.h>
#include <assimp/Importer.hpp>

#include <qrk/model.h>

namespace qrk {

Model::Model(const char* path) {
  std::string pathString(path);
  size_t i = pathString.find_last_of("/");
  // This will either be the model's directory, or empty string if the model is
  // at project root.
  directory_ = i != std::string::npos ? pathString.substr(0, i) : "";

  loadModel(pathString);
}

void Model::draw(Shader shader) {
  for (auto mesh : meshes_) {
    mesh.draw(shader);
  }
}

void Model::loadModel(std::string path) {
  Assimp::Importer importer;
  // Scene is freed by the importer.
  const aiScene* scene = importer.ReadFile(path, DEFAULT_LOAD_FLAGS);

  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
      !scene->mRootNode) {
    throw ModelLoaderException("ERROR::MODEL::" +
                               std::string(importer.GetErrorString()));
  }

  processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene) {
  // Process each mesh in the node.
  for (unsigned int i = 0; i < node->mNumMeshes; i++) {
    aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
    meshes_.push_back(processMesh(mesh, scene));
  }

  // Recurse for children. Recursion stops when no children left.
  for (unsigned int i = 0; i < node->mNumChildren; i++) {
    processNode(node->mChildren[i], scene);
  }
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene) {
  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;
  std::vector<Texture> textures;

  for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
    Vertex vertex;

    // Process vertex positions, normals, and texture coordinates.
    auto inputPos = mesh->mVertices[i];
    glm::vec3 position(inputPos.x, inputPos.y, inputPos.z);
    vertex.position = position;

    if (mesh->HasNormals()) {
      auto inputNorm = mesh->mNormals[i];
      glm::vec3 normal(inputNorm.x, inputNorm.y, inputNorm.z);
      vertex.normal = normal;
    } else {
      vertex.normal = glm::vec3(0.0f);
    }

    // TODO: This is only using the first texture coord set.
    if (mesh->HasTextureCoords(0)) {
      auto inputTexCoords = mesh->mTextureCoords[0][i];
      glm::vec2 texCoords(inputTexCoords.x, inputTexCoords.y);
      vertex.texCoords = texCoords;
    } else {
      vertex.texCoords = glm::vec2(0.0f);
    }

    vertices.push_back(vertex);
  }

  // Process indices.
  for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
    aiFace face = mesh->mFaces[i];
    for (unsigned int j = 0; j < face.mNumIndices; j++) {
      indices.push_back(face.mIndices[j]);
    }
  }

  // Process material.
  {
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    for (auto textureType : allTextureTypes()) {
      auto loadedMaps = loadMaterialTextures(material, textureType);
      textures.insert(textures.end(), loadedMaps.begin(), loadedMaps.end());
    }
  }

  return Mesh(vertices, indices, textures);
}

unsigned int loadTexture(const char* path) {
  unsigned int texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  // Set texture-wrapping/filtering options.
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  int width, height, numChannels;
  unsigned char* data = stbi_load(path, &width, &height, &numChannels, 0);

  if (data == nullptr) {
    stbi_image_free(data);
    throw ModelLoaderException("ERROR::MODEL::TEXTURE::LOAD_FAILED\n" +
                               std::string(path));
  }

  GLenum dataFormat;
  if (numChannels == 1) {
    dataFormat = GL_RED;
  } else if (numChannels == 3) {
    dataFormat = GL_RGB;
  } else if (numChannels == 4) {
    dataFormat = GL_RGBA;
  } else {
    throw ModelLoaderException(
        "ERROR::MODEL::TEXTURE::UNSUPPORTED_TEXTURE_FORMAT\n"
        "Texture '" +
        std::string(path) + "' contained unsupported number of channels: " +
        std::to_string(numChannels));
  }

  glTexImage2D(GL_TEXTURE_2D, /* mipmap level */ 0,
               /* texture format */ dataFormat, width, height, 0,
               /* tex data format */ dataFormat, GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);
  stbi_image_free(data);

  return texture;
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial* material,
                                                 TextureType type) {
  auto aiType = textureTypeToAiTextureType(type);
  std::vector<Texture> textures;

  for (unsigned int i = 0; i < material->GetTextureCount(aiType); i++) {
    aiString texturePath;
    material->GetTexture(aiType, i, &texturePath);
    // Assume that the texture path is relative to model directory.
    std::string fullPath = directory_ + "/" + texturePath.C_Str();

    // Don't re-load a texture if it's already been loaded.
    auto item = loadedTextures_.find(fullPath);
    if (item != loadedTextures_.end()) {
      // Texture has already been loaded.
      textures.push_back(item->second);
      continue;
    }

    Texture texture;
    texture.id = loadTexture(fullPath.c_str());
    texture.type = type;
    texture.path = fullPath;
    textures.push_back(texture);

    loadedTextures_[fullPath] = texture;
  }
  return textures;
}

}  // namespace qrk
