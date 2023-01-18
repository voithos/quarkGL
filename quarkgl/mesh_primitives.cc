#include "mesh_primitives.h"

#include <qrk/mesh_primitives.h>

namespace qrk {

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
  constexpr unsigned int planeVertexSizeBytes = 11 * sizeof(float);
  loadMeshData(planeVertices, sizeof(planeVertices) / planeVertexSizeBytes,
               planeVertexSizeBytes, /*indices=*/{}, textureMaps);
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
  constexpr unsigned int cubeVertexSizeBytes = 11 * sizeof(float);
  loadMeshData(cubeVertices, sizeof(cubeVertices) / cubeVertexSizeBytes,
               cubeVertexSizeBytes, /*indices=*/{}, textureMaps);
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
constexpr float roomVertices[] = {
    // positions           // normals            // tangents           // texture coords

    // back
    -0.5f, -0.5f, -0.5f,   0.0f,  0.0f,  1.0f,   1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
     0.5f, -0.5f, -0.5f,   0.0f,  0.0f,  1.0f,   1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
     0.5f,  0.5f, -0.5f,   0.0f,  0.0f,  1.0f,   1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,   0.0f,  0.0f,  1.0f,   1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,   0.0f,  0.0f,  1.0f,   1.0f,  0.0f,  0.0f,   1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,   0.0f,  0.0f,  1.0f,   1.0f,  0.0f,  0.0f,   1.0f, 0.0f,

    // front
    -0.5f, -0.5f,  0.5f,   0.0f,  0.0f, -1.0f,  -1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,   0.0f,  0.0f, -1.0f,  -1.0f,  0.0f,  0.0f,   1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,   0.0f,  0.0f, -1.0f,  -1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,   0.0f,  0.0f, -1.0f,  -1.0f,  0.0f,  0.0f,   1.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,   0.0f,  0.0f, -1.0f,  -1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,   0.0f,  0.0f, -1.0f,  -1.0f,  0.0f,  0.0f,   0.0f, 1.0f,

    // left
    -0.5f,  0.5f,  0.5f,   1.0f,  0.0f,  0.0f,   0.0f,  0.0f, -1.0f,   1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,   1.0f,  0.0f,  0.0f,   0.0f,  0.0f, -1.0f,   0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,   1.0f,  0.0f,  0.0f,   0.0f,  0.0f, -1.0f,   0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,   1.0f,  0.0f,  0.0f,   0.0f,  0.0f, -1.0f,   0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,   1.0f,  0.0f,  0.0f,   0.0f,  0.0f, -1.0f,   1.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,   1.0f,  0.0f,  0.0f,   0.0f,  0.0f, -1.0f,   1.0f, 0.0f,

    // right
     0.5f,  0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,   0.0f,  0.0f,  1.0f,   0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,   0.0f,  0.0f,  1.0f,   1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,   0.0f,  0.0f,  1.0f,   1.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,   0.0f,  0.0f,  1.0f,   1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,   0.0f,  0.0f,  1.0f,   0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,   0.0f,  0.0f,  1.0f,   0.0f, 1.0f,

    // bottom
    -0.5f, -0.5f, -0.5f,   0.0f,  1.0f,  0.0f,  -1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,   0.0f,  1.0f,  0.0f,  -1.0f,  0.0f,  0.0f,   1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,   0.0f,  1.0f,  0.0f,  -1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,   0.0f,  1.0f,  0.0f,  -1.0f,  0.0f,  0.0f,   1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,   0.0f,  1.0f,  0.0f,  -1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,   0.0f,  1.0f,  0.0f,  -1.0f,  0.0f,  0.0f,   0.0f, 1.0f,

    // top
    -0.5f,  0.5f, -0.5f,   0.0f, -1.0f,  0.0f,  -1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,   0.0f, -1.0f,  0.0f,  -1.0f,  0.0f,  0.0f,   1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,   0.0f, -1.0f,  0.0f,  -1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,   0.0f, -1.0f,  0.0f,  -1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,   0.0f, -1.0f,  0.0f,  -1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,   0.0f, -1.0f,  0.0f,  -1.0f,  0.0f,  0.0f,   0.0f, 1.0f
};
// clang-format on

RoomMesh::RoomMesh(std::string texturePath) {
  std::vector<TextureMap> textureMaps;
  if (!texturePath.empty()) {
    TextureMap textureMap(Texture::load(texturePath.c_str()),
                          TextureMapType::DIFFUSE);
    textureMaps.push_back(textureMap);
  }
  loadMeshAndTextures(textureMaps);
}

RoomMesh::RoomMesh(const std::vector<TextureMap>& textureMaps) {
  loadMeshAndTextures(textureMaps);
}

void RoomMesh::loadMeshAndTextures(const std::vector<TextureMap>& textureMaps) {
  constexpr unsigned int roomVertexSizeBytes = 11 * sizeof(float);
  loadMeshData(roomVertices, sizeof(roomVertices) / roomVertexSizeBytes,
               roomVertexSizeBytes, /*indices=*/{}, textureMaps);
}

void RoomMesh::initializeVertexAttributes() {
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

SphereMesh::SphereMesh(std::string texturePath, int numMeridians,
                       int numParallels)
    : numMeridians_(numMeridians), numParallels_(numParallels) {
  std::vector<TextureMap> textureMaps;
  if (!texturePath.empty()) {
    TextureMap textureMap(Texture::load(texturePath.c_str()),
                          TextureMapType::DIFFUSE);
    textureMaps.push_back(textureMap);
  }
  loadMeshAndTextures(textureMaps);
}

SphereMesh::SphereMesh(const std::vector<TextureMap>& textureMaps,
                       int numMeridians, int numParallels)
    : numMeridians_(numMeridians), numParallels_(numParallels) {
  loadMeshAndTextures(textureMaps);
}

void SphereMesh::loadMeshAndTextures(
    const std::vector<TextureMap>& textureMaps) {
  // Generate the sphere vertex components. This uses the common "UV" approach.

  const float PI = glm::pi<float>();

  // Always use at least 3 meridians.
  const unsigned int widthSegments = glm::max(numMeridians_, 3);
  // Add two segments to account for the poles.
  const unsigned int heightSegments = glm::max(numParallels_, 2);

  std::vector<float> vertexData;
  // We use <= instead of < because we want to create one more "layer" of
  // vertices in order for UVs to work properly.
  for (unsigned int iy = 0; iy <= heightSegments; ++iy) {
    // We can subdividue the surface into meridians / parallels, and treat
    // these as UVs since they lie in the range [0, 1].
    float v = iy / static_cast<float>(heightSegments);
    // However, we adjust it by a tiny amount to ensure that we don't generate
    // tangents of zero length (due to the sin() in the calculations).
    // TODO: Is this reasonable?
    v += 1e-5;
    // Use a special offset for the poles.
    float uOffset = 0.0f;
    if (iy == 0) {
      uOffset = 0.5f / widthSegments;
    } else if (iy == heightSegments) {
      uOffset = -0.5f / widthSegments;
    }
    for (unsigned int ix = 0; ix <= widthSegments; ++ix) {
      float u = ix / static_cast<float>(widthSegments);

      float x = cos(u * 2.0f * PI) * sin(v * PI);
      float y = cos(v * PI);
      float z = sin(u * 2.0f * PI) * sin(v * PI);

      // Positions.
      vertexData.push_back(x);
      vertexData.push_back(y);
      vertexData.push_back(z);
      // Normals. Unit sphere makes this easy. 8D
      vertexData.push_back(x);
      vertexData.push_back(y);
      vertexData.push_back(z);
      // Tangents. Rotated 90 degrees about the origin.
      vertexData.push_back(-z);
      vertexData.push_back(0.0f);  // Spherical tangents ignore parallels.
      vertexData.push_back(x);
      // Texture coordinates.
      vertexData.push_back(u + uOffset);
      vertexData.push_back(
          1.0f - v);  // We're generating the sphere top-down, so reverse V.
    }
  }

  std::vector<unsigned int> indices;
  // Since we created an extra duplicate "wraparound" vertex for each parallel,
  // we have to adjust the stride.
  const unsigned int widthStride = widthSegments + 1;
  for (unsigned int iy = 0; iy < heightSegments; ++iy) {
    for (unsigned int ix = 0; ix < widthSegments; ++ix) {
      // Form triangles from quad vertices.
      unsigned int a = iy * widthStride + (ix + 1);
      unsigned int b = iy * widthStride + ix;
      unsigned int c = (iy + 1) * widthStride + ix;
      unsigned int d = (iy + 1) * widthStride + (ix + 1);

      if (iy != 0) {
        indices.push_back(a);
        indices.push_back(b);
        indices.push_back(d);
      }
      if (iy != heightSegments - 1) {
        indices.push_back(b);
        indices.push_back(c);
        indices.push_back(d);
      }
    }
  }

  constexpr unsigned int sphereVertexSizeBytes = 11 * sizeof(float);
  loadMeshData(vertexData.data(),
               (sizeof(float) * vertexData.size()) / sphereVertexSizeBytes,
               sphereVertexSizeBytes, indices, textureMaps);
}

void SphereMesh::initializeVertexAttributes() {
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

SkyboxMesh::SkyboxMesh() { loadMesh(); }

SkyboxMesh::SkyboxMesh(std::vector<std::string> faces)
    : SkyboxMesh(Texture::loadCubemap(faces)) {}

SkyboxMesh::SkyboxMesh(Texture texture) {
  loadMesh();
  setTexture(texture);
}

void SkyboxMesh::setTexture(Attachment attachment) {
  setTexture(attachment.asTexture());
}

void SkyboxMesh::setTexture(Texture texture) {
  if (texture.getType() != TextureType::CUBEMAP) {
    throw MeshPrimitiveException(
        "ERROR::MESH_PRIMITIVE::INVALID_TEXTURE_TYPE\n" +
        std::to_string(static_cast<int>(texture.getType())));
  }
  // TODO: This copies the texture info, meaning it won't see updates.
  textureMaps_.clear();
  textureMaps_.emplace_back(texture, TextureMapType::CUBEMAP);
}

void SkyboxMesh::loadMesh() {
  constexpr unsigned int skyboxVertexSizeBytes = 3 * sizeof(float);
  loadMeshData(skyboxVertices, sizeof(skyboxVertices) / skyboxVertexSizeBytes,
               skyboxVertexSizeBytes, /*indices=*/{}, /*textureMaps=*/{});
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

ScreenQuadMesh::ScreenQuadMesh() { loadMesh(); }

ScreenQuadMesh::ScreenQuadMesh(Texture texture) {
  loadMesh();
  setTexture(texture);
}

void ScreenQuadMesh::loadMesh() {
  constexpr unsigned int quadVertexSizeBytes = 4 * sizeof(float);
  loadMeshData(screenQuadVertices,
               sizeof(screenQuadVertices) / quadVertexSizeBytes,
               quadVertexSizeBytes, /*indices=*/{}, /*textureMaps=*/{});
}

void ScreenQuadMesh::setTexture(Attachment attachment) {
  setTexture(attachment.asTexture());
}

void ScreenQuadMesh::setTexture(Texture texture) {
  // TODO: This copies the texture info, meaning it won't see updates.
  textureMaps_.clear();
  textureMaps_.emplace_back(texture, TextureMapType::DIFFUSE);
}

void ScreenQuadMesh::unsetTexture() { textureMaps_.clear(); }

void ScreenQuadMesh::initializeVertexAttributes() {
  // Screen positions.
  vertexArray_.addVertexAttrib(2, GL_FLOAT);
  // Texture coordinates.
  vertexArray_.addVertexAttrib(2, GL_FLOAT);
  vertexArray_.finalizeVertexAttribs();
}

void ScreenQuadMesh::bindTextures(Shader& shader,
                                  TextureRegistry* textureRegistry) {
  if (textureMaps_.empty()) {
    return;
  }

  // Bind textures, assuming a given uniform naming.
  // If a TextureRegistry isn't provided, just start with texture unit 0.
  unsigned int textureUnit = 0;
  if (textureRegistry != nullptr) {
    textureRegistry->pushUsageBlock();
    textureUnit = textureRegistry->getNextTextureUnit();
  }

  Texture& texture = textureMaps_[0].getTexture();
  texture.bindToUnit(textureUnit, TextureBindType::TEXTURE_2D);

  // Set the sampler to the correct texture unit.
  shader.setInt("qrk_screenTexture", textureUnit);
  if (textureRegistry != nullptr) {
    textureRegistry->popUsageBlock();
  }
}

}  // namespace qrk
