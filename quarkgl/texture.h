#ifndef QUARKGL_TEXTURE_H_
#define QUARKGL_TEXTURE_H_

#include <qrk/exceptions.h>

#include <string>
#include <vector>

namespace qrk {

class TextureException : public QuarkException {
  using QuarkException::QuarkException;
};

// The type of texture binding.
enum class TextureBindType {
  // A normal TEXTURE_2D.
  TEXTURE = 0,
  // A cubemap.
  CUBEMAP,
  // An image texture that is directly indexed, rather than sampled.
  IMAGE_TEXTURE,
};

enum class TextureFiltering {
  // Uses nearest-neighbor sampling.
  NEAREST = 0,
  // Uses linear interpolation between texels.
  BILINEAR,
  // Uses linear interpolation between mipmap levels and their texels.
  TRILINEAR,
  // Handles anisotropy when sampling.
  ANISOTROPIC,
};

enum class TextureWrapMode {
  REPEAT = 0,
  CLAMP_TO_EDGE,
  // TODO: Add the others?
};

struct TextureParams {
  TextureFiltering filtering = TextureFiltering::NEAREST;
  TextureWrapMode wrapMode = TextureWrapMode::REPEAT;
};

class Texture {
 public:
  // Loads a texture from a given path.
  // TODO: Consider putting this in a TextureLoader class.
  static Texture load(const char* path, bool isSRGB = true);
  static Texture load(const char* path, bool isSRGB,
                      const TextureParams& params);

  // Loads a cubemap from a set of 6 textures for the faces. Textures must be
  // passed in order starting with GL_TEXTURE_CUBE_MAP_POSITIVE_X and
  // incrementing from there; namely, in the order right, left, top, bottom,
  // front, and back.
  static Texture loadCubemap(std::vector<std::string> faces);

  // Creates a custom texture of the given size and format.
  static Texture create(int width, int height, GLenum internalFormat);
  static Texture create(int width, int height, GLenum internalFormat,
                        const TextureParams& params);

  // Binds the texture to the given texture unit.
  // Unit should be a number starting from 0, not the actual texture unit's
  // GLenum. This will bind samplers normally, but will bind cubemaps as
  // cubemaps and custom texture as image textures.
  void bindToUnit(unsigned int textureUnit,
                  TextureBindType bindType = TextureBindType::TEXTURE);

  unsigned int getId() const { return id_; }
  // Returns the path to a texture. Not applicable for cubemaps or generated
  // textures.
  std::string getPath() const { return path_; }
  int getWidth() const { return width_; }
  int getHeight() const { return height_; }
  int getNumChannels() const { return numChannels_; }
  // TODO: Remove GLenum from this API (use a custom enum).
  GLenum getInternalFormat() const { return internalFormat_; }

 private:
  unsigned int id_;
  std::string path_;
  int width_;
  int height_;
  int numChannels_;
  GLenum internalFormat_;

  // Applies the given params to the currently-active texture. Assumes the
  // texture is a 2d texture (e.g. not a cubemap).
  static void applyParams(const TextureParams& params);
};
}  // namespace qrk

#endif
