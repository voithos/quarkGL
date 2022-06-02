#include <qrk/mesh.h>

namespace qrk {
Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices,
           std::vector<Texture> textures, unsigned int instanceCount)
    : vertices_(vertices),
      indices_(indices),
      textures_(textures),
      instanceCount_(instanceCount) {
  // Load VBO.
  vertexArray_.loadVertexData(&vertices_[0], vertices.size() * sizeof(Vertex));

  // Positions.
  vertexArray_.addVertexAttrib(3, GL_FLOAT);
  // Normals.
  // TODO: Allow drawing meshes that don't have normals.
  vertexArray_.addVertexAttrib(3, GL_FLOAT);
  // Texture coordinates.
  vertexArray_.addVertexAttrib(2, GL_FLOAT);

  vertexArray_.finalizeVertexAttribs();

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

  // Load EBO.
  vertexArray_.loadElementData(&indices_[0],
                               indices.size() * sizeof(unsigned int));
}

void Mesh::loadInstanceModels(const std::vector<glm::mat4>& models) {
  vertexArray_.loadInstanceVertexData(&models[0],
                                      models.size() * sizeof(glm::mat4));
}

void Mesh::loadInstanceModels(const glm::mat4* models, unsigned int size) {
  vertexArray_.loadInstanceVertexData(&models[0], size * sizeof(glm::mat4));
}

void Mesh::draw(Shader shader) {
  // Bind textures. Assumes uniform naming is "material.textureType[idx]".
  unsigned int diffuseIdx = 0;
  unsigned int specularIdx = 0;
  unsigned int emissionIdx = 0;

  unsigned int textureUnit = 0;
  for (const Texture& texture : textures_) {
    // TODO: Take into account GL_MAX_TEXTURE_UNITS here.
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_2D, texture.id);

    std::ostringstream ss;
    ss << "material.";

    switch (texture.type) {
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
    }

    // Set the sampler to the correct texture unit.
    shader.setInt(ss.str(), textureUnit);
    textureUnit++;
  }
  shader.setInt("material.diffuseCount", diffuseIdx);
  shader.setInt("material.specularCount", specularIdx);
  shader.setInt("material.emissionCount", emissionIdx);

  // Draw using the VAO.
  shader.activate();
  vertexArray_.activate();

  // Handle instancing.
  if (instanceCount_) {
    glDrawElementsInstanced(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_INT,
                            nullptr, instanceCount_);
  } else {
    glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_INT, nullptr);
  }

  vertexArray_.deactivate();

  // Reset.
  glActiveTexture(GL_TEXTURE0);
}
}  // namespace qrk
