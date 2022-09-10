#include <qrk/mesh_primitives.h>

namespace qrk {

void PrimitiveMesh::draw(Shader& shader, TextureRegistry* textureRegistry) {
  // First we set the model transform.
  shader.setMat4("model", getModelTransform());

  // Then render as normal.
  Mesh::draw(shader, textureRegistry);
}

// clang-format off
constexpr float planeVertices[] = {
    // positions           // normals            // tangents           // texture coords
    -0.5f,  0.0f, -0.5f,   0.0f,  1.0f,  0.0f,   1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
     0.5f,  0.0f,  0.5f,   0.0f,  1.0f,  0.0f,   1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
     0.5f,  0.0f, -0.5f,   0.0f,  1.0f,  0.0f,   1.0f,  0.0f,  0.0f,   1.0f, 1.0f,

     0.5f,  0.0f,  0.5f,   0.0f,  1.0f,  0.0f,   1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
    -0.5f,  0.0f, -0.5f,   0.0f,  1.0f,  0.0f,   1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
    -0.5f,  0.0f,  0.5f,   0.0f,  1.0f,  0.0f,   1.0f,  0.0f,  0.0f,   0.0f, 0.0f
};
// clang-format on

PlaneMesh::PlaneMesh(std::string texturePath) {
  std::vector<TextureMap> textureMaps;
  if (!texturePath.empty()) {
    TextureMap textureMap(Texture::load(texturePath.c_str()),
                          TextureMapType::DIFFUSE);
    textureMaps.push_back(textureMap);
  }
  loadMeshAndTextures(textureMaps);
}

PlaneMesh::PlaneMesh(const std::vector<TextureMap>& textureMaps) {
  loadMeshAndTextures(textureMaps);
}

void PlaneMesh::loadMeshAndTextures(
    const std::vector<TextureMap>& textureMaps) {
  constexpr unsigned int planeVertexSize = 11;
  loadMeshData(planeVertices, sizeof(planeVertices) / planeVertexSize,
               planeVertexSize, /*indices=*/{}, textureMaps);
}

void PlaneMesh::initializeVertexAttributes() {
  // Positions.
  vertexArray_.addVertexAttrib(3, GL_FLOAT);
  // Normals.
  vertexArray_.addVertexAttrib(3, GL_FLOAT);
  // Tangents.
  vertexArray_.addVertexAttrib(3, GL_FLOAT);
  // Texture coordinates.
  vertexArray_.addVertexAttrib(2, GL_FLOAT);

  vertexArray_.finalizeVertexAttribs();
}

// clang-format off
constexpr float cubeVertices[] = {
    // positions           // normals            // tangents           // texture coords

    // back
    -0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,  -1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,  -1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,  -1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
     0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,  -1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,  -1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,  -1.0f,  0.0f,  0.0f,   1.0f, 1.0f,

    // front
    -0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f,   1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f,   1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,   0.0f,  0.0f,  1.0f,   1.0f,  0.0f,  0.0f,   1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,   0.0f,  0.0f,  1.0f,   1.0f,  0.0f,  0.0f,   1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,   0.0f,  0.0f,  1.0f,   1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f,   1.0f,  0.0f,  0.0f,   0.0f, 0.0f,

    // left
    -0.5f,  0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,   0.0f,  0.0f,  1.0f,   1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,   0.0f,  0.0f,  1.0f,   0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,   0.0f,  0.0f,  1.0f,   0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,   0.0f,  0.0f,  1.0f,   0.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,   0.0f,  0.0f,  1.0f,   1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,   0.0f,  0.0f,  1.0f,   1.0f, 1.0f,

    // right
     0.5f,  0.5f,  0.5f,   1.0f,  0.0f,  0.0f,   0.0f,  0.0f, -1.0f,   0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,   1.0f,  0.0f,  0.0f,   0.0f,  0.0f, -1.0f,   1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,   1.0f,  0.0f,  0.0f,   0.0f,  0.0f, -1.0f,   1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,   1.0f,  0.0f,  0.0f,   0.0f,  0.0f, -1.0f,   1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,   1.0f,  0.0f,  0.0f,   0.0f,  0.0f, -1.0f,   0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,   1.0f,  0.0f,  0.0f,   0.0f,  0.0f, -1.0f,   0.0f, 0.0f,

    // bottom
    -0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,   1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,   1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,   1.0f,  0.0f,  0.0f,   1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,   1.0f,  0.0f,  0.0f,   1.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,   1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,   1.0f,  0.0f,  0.0f,   0.0f, 0.0f,

    // top
    -0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,   1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,   1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,   1.0f,  0.0f,  0.0f,   1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,   1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,   1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,   1.0f,  0.0f,  0.0f,   0.0f, 0.0f
};
// clang-format on

CubeMesh::CubeMesh(std::string texturePath) {
  std::vector<TextureMap> textureMaps;
  if (!texturePath.empty()) {
    TextureMap textureMap(Texture::load(texturePath.c_str()),
                          TextureMapType::DIFFUSE);
    textureMaps.push_back(textureMap);
  }
  loadMeshAndTextures(textureMaps);
}

CubeMesh::CubeMesh(const std::vector<TextureMap>& textureMaps) {
  loadMeshAndTextures(textureMaps);
}

void CubeMesh::loadMeshAndTextures(const std::vector<TextureMap>& textureMaps) {
  constexpr unsigned int cubeVertexSize = 11;
  loadMeshData(cubeVertices, sizeof(cubeVertices) / cubeVertexSize,
               cubeVertexSize, /*indices=*/{}, textureMaps);
}
void CubeMesh::initializeVertexAttributes() {
  // Positions.
  vertexArray_.addVertexAttrib(3, GL_FLOAT);
  // Normals.
  vertexArray_.addVertexAttrib(3, GL_FLOAT);
  // Tangents.
  vertexArray_.addVertexAttrib(3, GL_FLOAT);
  // Texture coordinates.
  vertexArray_.addVertexAttrib(2, GL_FLOAT);

  vertexArray_.finalizeVertexAttribs();
}

// clang-format off
constexpr float skyboxVertices[] = {
    // positions          
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f
};
// clang-format on

SkyboxMesh::SkyboxMesh(std::vector<std::string> faces) {
  TextureMap textureMap(Texture::loadCubemap(faces), TextureMapType::CUBEMAP);
  constexpr unsigned int skyboxVertexSize = 3;
  loadMeshData(skyboxVertices, sizeof(skyboxVertices) / skyboxVertexSize,
               skyboxVertexSize, /*indices=*/{}, {textureMap});
}

void SkyboxMesh::initializeVertexAttributes() {
  // Skybox only has vertex positions.
  vertexArray_.addVertexAttrib(3, GL_FLOAT);
  vertexArray_.finalizeVertexAttribs();
}

// clang-format off
constexpr float screenQuadVertices[] = {
    // positions   // texture coords
    -1.0f,  1.0f,  0.0f, 1.0f,
    -1.0f, -1.0f,  0.0f, 0.0f,
     1.0f, -1.0f,  1.0f, 0.0f,

    -1.0f,  1.0f,  0.0f, 1.0f,
     1.0f, -1.0f,  1.0f, 0.0f,
     1.0f,  1.0f,  1.0f, 1.0f
};
// clang-format on

ScreenQuadMesh::ScreenQuadMesh(Texture texture) {
  constexpr unsigned int quadVertexSize = 4;
  // TODO: This copies the texture info, meaning it won't see updates.
  TextureMap textureMap(texture, TextureMapType::DIFFUSE);
  loadMeshData(screenQuadVertices, sizeof(screenQuadVertices) / quadVertexSize,
               quadVertexSize, /*indices=*/{}, {textureMap});
}

void ScreenQuadMesh::initializeVertexAttributes() {
  // Screen positions.
  vertexArray_.addVertexAttrib(2, GL_FLOAT);
  // Texture coordinates.
  vertexArray_.addVertexAttrib(2, GL_FLOAT);
  vertexArray_.finalizeVertexAttribs();
}

}  // namespace qrk
