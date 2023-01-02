#ifndef QUARKGL_MESH_PRIMITIVES_H_
#define QUARKGL_MESH_PRIMITIVES_H_

#include <qrk/framebuffer.h>
#include <qrk/mesh.h>
#include <qrk/texture.h>
#include <qrk/texture_map.h>

namespace qrk {

class PrimitiveMesh : public Mesh {};

class PlaneMesh : public PrimitiveMesh {
 public:
  explicit PlaneMesh(std::string texturePath = "");
  explicit PlaneMesh(const std::vector<TextureMap>& textureMaps);

 protected:
  void loadMeshAndTextures(const std::vector<TextureMap>& textureMaps);
  void initializeVertexAttributes() override;
};

// A unit cube.
class CubeMesh : public PrimitiveMesh {
 public:
  explicit CubeMesh(std::string texturePath = "");
  explicit CubeMesh(const std::vector<TextureMap>& textureMaps);

 protected:
  void loadMeshAndTextures(const std::vector<TextureMap>& textureMaps);
  void initializeVertexAttributes() override;
};

// Like CubeMesh, but with normals pointing inward.
class RoomMesh : public PrimitiveMesh {
 public:
  explicit RoomMesh(std::string texturePath = "");
  explicit RoomMesh(const std::vector<TextureMap>& textureMaps);

 protected:
  void loadMeshAndTextures(const std::vector<TextureMap>& textureMaps);
  void initializeVertexAttributes() override;
};

// A unit sphere, with the given number of meridians / parallels.
class SphereMesh : public PrimitiveMesh {
 public:
  static constexpr int DEFAULT_NUM_MERIDIANS = 64;
  static constexpr int DEFAULT_NUM_PARALLELS = 64;

  SphereMesh(std::string texturePath = "",
             int numMeridians = DEFAULT_NUM_MERIDIANS,
             int numParallels = DEFAULT_NUM_PARALLELS);
  SphereMesh(const std::vector<TextureMap>& textureMaps,
             int numMeridians = DEFAULT_NUM_MERIDIANS,
             int numParallels = DEFAULT_NUM_PARALLELS);

 protected:
  void loadMeshAndTextures(const std::vector<TextureMap>& textureMaps);
  void initializeVertexAttributes() override;

  int numMeridians_;
  int numParallels_;
};

class SkyboxMesh : public PrimitiveMesh {
 public:
  // Creates a new skybox mesh from a set of 6 textures for the faces. Textures
  // must be passed in order starting with GL_TEXTURE_CUBE_MAP_POSITIVE_X and
  // incrementing from there; namely, in the order right, left, top, bottom,
  // front, and back.
  explicit SkyboxMesh(std::vector<std::string> faces);

 protected:
  void initializeVertexAttributes() override;
};

class ScreenQuadMesh : public PrimitiveMesh {
 public:
  // Creates an unbound screen quad mesh.
  ScreenQuadMesh();
  // Creates a new screen quad mesh from a texture.
  explicit ScreenQuadMesh(Texture texture);

  // Sets a framebuffer attachment as the texture.
  void setTexture(Attachment attachment);
  // Sets the texture. This overrides previously set textures.
  void setTexture(Texture texture);
  // Unsets the texture.
  void unsetTexture();

 protected:
  void loadMesh();
  void initializeVertexAttributes() override;
  void bindTextures(Shader& shader, TextureRegistry* textureRegistry) override;
};

}  // namespace qrk

#endif
