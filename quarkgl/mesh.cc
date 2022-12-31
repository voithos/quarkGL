#include <qrk/mesh.h>

namespace qrk {

void RenderableNode::drawWithTransform(const glm::mat4& transform,
                                       Shader& shader,
                                       TextureRegistry* textureRegistry) {
  // Combined incoming transform with the node's.
  const glm::mat4 mat = transform * getModelTransform();
  for (auto& renderable : renderables_) {
    renderable->drawWithTransform(mat, shader, textureRegistry);
  }

  // Render children.
  for (auto& childNode : childNodes_) {
    childNode->drawWithTransform(mat, shader, textureRegistry);
  }
}

void RenderableNode::visitRenderables(
    std::function<void(Renderable*)> visitor) {
  for (auto& renderable : renderables_) {
    visitor(renderable.get());
  }
  for (auto& childNode : childNodes_) {
    childNode->visitRenderables(visitor);
  }
}

void Mesh::loadMeshData(const void* vertexData, unsigned int numVertices,
                        unsigned int vertexSizeBytes,
                        const std::vector<unsigned int>& indices,
                        const std::vector<TextureMap>& textureMaps,
                        unsigned int instanceCount) {
  indices_ = indices;
  textureMaps_ = textureMaps;
  numVertices_ = numVertices;
  vertexSizeBytes_ = vertexSizeBytes;
  instanceCount_ = instanceCount;

  // Load VBO.
  vertexArray_.loadVertexData(vertexData, numVertices_ * vertexSizeBytes);

  initializeVertexAttributes();
  initializeVertexArrayInstanceData();

  // Load EBO if this is an indexed mesh.
  if (!indices_.empty()) {
    vertexArray_.loadElementData(&indices_[0],
                                 indices.size() * sizeof(unsigned int));
  }
}

void Mesh::loadInstanceModels(const std::vector<glm::mat4>& models) {
  vertexArray_.loadInstanceVertexData(&models[0],
                                      models.size() * sizeof(glm::mat4));
}

void Mesh::loadInstanceModels(const glm::mat4* models, unsigned int size) {
  vertexArray_.loadInstanceVertexData(&models[0], size * sizeof(glm::mat4));
}

void Mesh::drawWithTransform(const glm::mat4& transform, Shader& shader,
                             TextureRegistry* textureRegistry) {
  // First we set the model transform, combining with the incoming transform.
  shader.setMat4("model", transform * getModelTransform());

  bindTextures(shader, textureRegistry);

  // Draw using the VAO.
  shader.activate();
  vertexArray_.activate();

  glDraw();

  vertexArray_.deactivate();

  // Reset.
  shader.deactivate();
}

void Mesh::initializeVertexArrayInstanceData() {
  if (instanceCount_) {
    // Allocate space for mat4 model transforms for the instancing.
    vertexArray_.allocateInstanceVertexData(instanceCount_ * sizeof(glm::mat4));
    // Add vertex attributes (max attribute size is vec4, so we need 4 of them).
    vertexArray_.addVertexAttrib(4, GL_FLOAT, /*instanceDivisor=*/1);
    vertexArray_.addVertexAttrib(4, GL_FLOAT, /*instanceDivisor=*/1);
    vertexArray_.addVertexAttrib(4, GL_FLOAT, /*instanceDivisor=*/1);
    vertexArray_.addVertexAttrib(4, GL_FLOAT, /*instanceDivisor=*/1);
    vertexArray_.finalizeVertexAttribs();
  }
}

void Mesh::bindTextures(Shader& shader, TextureRegistry* textureRegistry) {
  // Bind textures. Assumes uniform naming is "material.textureMapType[idx]".
  unsigned int diffuseIdx = 0;
  unsigned int specularIdx = 0;
  unsigned int roughnessIdx = 0;
  unsigned int metallicIdx = 0;
  unsigned int aoIdx = 0;
  unsigned int emissionIdx = 0;
  bool hasNormalMap = false;

  // If a TextureRegistry isn't provided, just start with texture unit 0.
  unsigned int textureUnit = 0;
  if (textureRegistry != nullptr) {
    textureRegistry->pushUsageBlock();
    textureUnit = textureRegistry->getNextTextureUnit();
  }
  for (TextureMap& textureMap : textureMaps_) {
    std::string samplerName;
    TextureMapType type = textureMap.getType();
    Texture& texture = textureMap.getTexture();
    if (type == TextureMapType::CUBEMAP) {
      texture.bindToUnit(textureUnit, TextureBindType::CUBEMAP);
      samplerName = "skybox";
    } else {
      texture.bindToUnit(textureUnit, TextureBindType::TEXTURE);
      std::ostringstream ss;
      // TODO: Make this more configurable / less generic?
      ss << "material.";

      switch (type) {
        case TextureMapType::DIFFUSE:
          ss << "diffuseMaps[" << diffuseIdx << "]";
          diffuseIdx++;
          break;
        case TextureMapType::SPECULAR:
          ss << "specularMaps[" << specularIdx << "]";
          specularIdx++;
          break;
        case TextureMapType::ROUGHNESS:
          ss << "roughnessMaps[" << roughnessIdx << "]";
          roughnessIdx++;
          break;
        case TextureMapType::METALLIC:
          ss << "metallicMaps[" << metallicIdx << "]";
          metallicIdx++;
          break;
        case TextureMapType::AO:
          ss << "aoMaps[" << aoIdx << "]";
          aoIdx++;
          break;
        case TextureMapType::EMISSION:
          ss << "emissionMaps[" << emissionIdx << "]";
          emissionIdx++;
          break;
        case TextureMapType::NORMAL:
          // Only a single normal map supported.
          ss << "normalMap";
          hasNormalMap = true;
          break;
        case TextureMapType::CUBEMAP:
          // Handled earlier.
          abort();
          break;
      }
      samplerName = ss.str();
    }
    // Set the sampler to the correct texture unit.
    shader.setInt(samplerName, textureUnit);

    if (textureRegistry != nullptr) {
      textureUnit = textureRegistry->getNextTextureUnit();
    } else {
      textureUnit++;
    }
  }
  if (textureRegistry != nullptr) {
    textureRegistry->popUsageBlock();
  }
  shader.setInt("material.diffuseCount", diffuseIdx);
  shader.setInt("material.specularCount", specularIdx);
  shader.setInt("material.roughnessCount", roughnessIdx);
  shader.setInt("material.metallicCount", metallicIdx);
  shader.setInt("material.aoCount", aoIdx);
  shader.setInt("material.emissionCount", emissionIdx);
  shader.setInt("material.hasNormalMap", hasNormalMap);
}

void Mesh::glDraw() {
  // Handle instancing.
  if (instanceCount_) {
    // Handle indexed arrays.
    if (!indices_.empty()) {
      glDrawElementsInstanced(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_INT,
                              nullptr, instanceCount_);
    } else {
      glDrawArraysInstanced(GL_TRIANGLES, 0, numVertices_, instanceCount_);
    }

  } else {
    // Handle indexed arrays.
    if (!indices_.empty()) {
      glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_INT, nullptr);
    } else {
      glDrawArrays(GL_TRIANGLES, 0, numVertices_);
    }
  }
}
}  // namespace qrk
