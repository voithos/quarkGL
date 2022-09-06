#include <glad/glad.h>
#include <qrk/model.h>

#include <assimp/Importer.hpp>

namespace qrk {
namespace {
constexpr TextureMapType loaderSupportedTextureMapTypes[] = {
    TextureMapType::DIFFUSE,
    TextureMapType::SPECULAR,
    TextureMapType::EMISSION,
};
}

ModelMesh::ModelMesh(const std::vector<ModelVertex>& vertices,
                     const std::vector<unsigned int>& indices,
                     const std::vector<TextureMap>& textureMaps,
                     unsigned int instanceCount)
    : vertices_(vertices) {
  loadMeshData(&vertices_[0], vertices_.size(), sizeof(ModelVertex), indices,
               textureMaps, instanceCount);
}

void ModelMesh::initializeVertexAttributes() {
  // Positions.
  vertexArray_.addVertexAttrib(3, GL_FLOAT);
  // Normals.
  // TODO: Allow drawing models that don't have normals.
  vertexArray_.addVertexAttrib(3, GL_FLOAT);
  // Texture coordinates.
  vertexArray_.addVertexAttrib(2, GL_FLOAT);

  vertexArray_.finalizeVertexAttribs();
}

Model::Model(const char* path, unsigned int instanceCount)
    : instanceCount_(instanceCount) {
  std::string pathString(path);
  size_t i = pathString.find_last_of("/");
  // This will either be the model's directory, or empty string if the model is
  // at project root.
  directory_ = i != std::string::npos ? pathString.substr(0, i) : "";

  loadModel(pathString);
}

void Model::loadInstanceModels(const std::vector<glm::mat4>& models) {
  for (auto mesh : meshes_) {
    mesh.loadInstanceModels(models);
  }
}

void Model::loadInstanceModels(const glm::mat4* models, unsigned int size) {
  for (auto mesh : meshes_) {
    mesh.loadInstanceModels(models, size);
  }
}

void Model::draw(Shader& shader, TextureRegistry* textureRegistry) {
  // First we set the model transform.
  shader.setMat4("model", getModelTransform());

  for (auto mesh : meshes_) {
    mesh.draw(shader, textureRegistry);
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

ModelMesh Model::processMesh(aiMesh* mesh, const aiScene* scene) {
  std::vector<ModelVertex> vertices;
  std::vector<unsigned int> indices;
  std::vector<TextureMap> textureMaps;

  for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
    ModelVertex vertex;

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
    for (auto type : loaderSupportedTextureMapTypes) {
      auto loadedMaps = loadMaterialTextureMaps(material, type);
      textureMaps.insert(textureMaps.end(), loadedMaps.begin(),
                         loadedMaps.end());
    }
  }

  return ModelMesh(vertices, indices, textureMaps, instanceCount_);
}

std::vector<TextureMap> Model::loadMaterialTextureMaps(aiMaterial* material,
                                                       TextureMapType type) {
  auto aiType = textureMapTypeToAiTextureType(type);
  std::vector<TextureMap> textureMaps;

  for (unsigned int i = 0; i < material->GetTextureCount(aiType); i++) {
    aiString texturePath;
    material->GetTexture(aiType, i, &texturePath);
    // TODO: Pull the texture loading bits into a separate class.
    // Assume that the texture path is relative to model directory.
    std::string fullPath = directory_ + "/" + texturePath.C_Str();

    // Don't re-load a texture if it's already been loaded.
    auto item = loadedTextureMaps_.find(fullPath);
    if (item != loadedTextureMaps_.end()) {
      // Texture has already been loaded.
      textureMaps.push_back(item->second);
      continue;
    }

    // Assume that diffuse textures are in sRGB.
    // TODO: Allow for a way to override this if necessary.
    bool isSRGB = type == TextureMapType::DIFFUSE;

    TextureMap textureMap(Texture::load(fullPath.c_str(), isSRGB), type);
    textureMaps.push_back(textureMap);
    loadedTextureMaps_.insert(std::make_pair(fullPath, textureMap));
  }
  return textureMaps;
}

}  // namespace qrk
