#include <qrk/mesh.h>

namespace qrk {
void Mesh::loadMeshData(const void* vertexData, unsigned int numVertices,
                        unsigned int vertexSize,
                        std::vector<unsigned int> indices,
                        std::vector<Texture> textures,
                        unsigned int instanceCount) {
  indices_ = indices;
  textures_ = textures;
  numVertices_ = numVertices;
  vertexSize_ = vertexSize;
  instanceCount_ = instanceCount;

  // Load VBO.
  vertexArray_.loadVertexData(vertexData, numVertices_ * vertexSize_);

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

void Mesh::draw(Shader& shader) {
  // Note: Model transform handling is done by child classes.

  bindTextures(shader);

  // Draw using the VAO.
  shader.activate();
  vertexArray_.activate();

  glDraw();

  vertexArray_.deactivate();

  // Reset.
  shader.deactivate();
  glActiveTexture(GL_TEXTURE0);
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

void Mesh::bindTextures(Shader& shader) {
  // Bind textures. Assumes uniform naming is "material.textureType[idx]".
  unsigned int diffuseIdx = 0;
  unsigned int specularIdx = 0;
  unsigned int emissionIdx = 0;

  unsigned int textureUnit = 0;
  for (const Texture& texture : textures_) {
    // TODO: Take into account GL_MAX_TEXTURE_UNITS here.
    glActiveTexture(GL_TEXTURE0 + textureUnit);

    std::string samplerName;
    TextureType type = texture.getType();
    if (type == TextureType::CUBEMAP) {
      glBindTexture(GL_TEXTURE_CUBE_MAP, texture.getId());
      samplerName = "skybox";
    } else {
      glBindTexture(GL_TEXTURE_2D, texture.getId());

      std::ostringstream ss;
      ss << "material.";

      switch (type) {
        case TextureType::DIFFUSE:
          ss << "diffuse[" << diffuseIdx << "]";
          diffuseIdx++;
          break;
        case TextureType::SPECULAR:
          ss << "specular[" << specularIdx << "]";
          specularIdx++;
          break;
        case TextureType::EMISSION:
          ss << "emission[" << emissionIdx << "]";
          emissionIdx++;
          break;
        case TextureType::CUBEMAP:
          // Handled earlier.
          abort();
          break;
      }
      samplerName = ss.str();
    }

    // Set the sampler to the correct texture unit.
    shader.setInt(samplerName, textureUnit);
    textureUnit++;
  }
  shader.setInt("material.diffuseCount", diffuseIdx);
  shader.setInt("material.specularCount", specularIdx);
  shader.setInt("material.emissionCount", emissionIdx);
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
