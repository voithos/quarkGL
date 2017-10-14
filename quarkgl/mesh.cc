#include <qrk/mesh.h>

namespace qrk {
Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices,
           std::vector<Texture> textures)
    : vertices_(vertices), indices_(indices), textures_(textures) {
  // Load VBO.
  vertexArray_.loadVertexData(&vertices_[0], vertices.size() * sizeof(Vertex));

  // Positions.
  vertexArray_.addVertexAttrib(3, GL_FLOAT);
  // Normals.
  vertexArray_.addVertexAttrib(3, GL_FLOAT);
  // Texture coordinates.
  vertexArray_.addVertexAttrib(2, GL_FLOAT);

  vertexArray_.finalizeVertexAttribs();

  // Load EBO.
  vertexArray_.loadElementData(&indices_[0],
                               indices.size() * sizeof(unsigned int));
}

void Mesh::draw(Shader shader) {
  // Bind textures. Assumes uniform naming is "material.textureType[idx]".
  unsigned int diffuseIdx = 0;
  unsigned int specularIdx = 0;
  unsigned int emissionIdx = 0;

  unsigned int textureIdx = 0;
  for (const Texture& texture : textures_) {
    // TODO: Take into account GL_MAX_TEXTURE_UNITS here.
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

    shader.setInt(ss.str(), texture.id);
    textureIdx++;
  }
  shader.setInt("material.diffuseCount", diffuseIdx);
  shader.setInt("material.specularCount", specularIdx);
  shader.setInt("material.emissionCount", emissionIdx);

  // Draw using the VAO.
  shader.activate();
  vertexArray_.activate();
  glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_INT, nullptr);
  vertexArray_.deactivate();

  // Reset.
  glActiveTexture(GL_TEXTURE0);
}
}  // namespace qrk
