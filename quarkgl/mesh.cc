#include <qrk/mesh.h>

namespace qrk {
Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices,
           std::vector<Texture> textures)
    : vertices_(vertices), indices_(indices), textures_(textures) {
  // Load VBO and EBO.
  vertexArray_.loadVertexData(&vertices_[0], vertices.size() * sizeof(Vertex));
  vertexArray_.loadElementData(&indices_[0],
                               indices.size() * sizeof(unsigned int));

  // Positions.
  vertexArray_.addVertexAttrib(sizeof(glm::vec3), GL_FLOAT);
  // Normals.
  vertexArray_.addVertexAttrib(sizeof(glm::vec3), GL_FLOAT);
  // Texture coordinates.
  vertexArray_.addVertexAttrib(sizeof(glm::vec2), GL_FLOAT);

  vertexArray_.finalizeVertexAttribs();
}

void Mesh::draw(Shader shader) {
  // Bind textures. Assumes uniform naming is "material.textureType[idx]".
  unsigned int diffuseIdx = 0;
  unsigned int specularIdx = 0;
  unsigned int emissionIdx = 0;

  int textureIdx = 0;
  for (const Texture& texture : textures_) {
    glActiveTexture(GL_TEXTURE0 + textureIdx);
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

    shader.setInt(ss.str().c_str(), texture.id);
    textureIdx++;
  }
  glActiveTexture(GL_TEXTURE0);

  // Draw using the VAO.
  vertexArray_.use();
  glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_INT, 0);
  vertexArray_.unuse();
}
}
