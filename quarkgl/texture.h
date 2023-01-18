#ifndef QUARKGL_TEXTURE_H_
#define QUARKGL_TEXTURE_H_

#include <qrk/exceptions.h>
#include <qrk/screen.h>

#include <glm/glm.hpp>
#include <string>
#include <vector>

namespace qrk {

class TextureException : public QuarkException {
  using QuarkException::QuarkException;
};

// Texture type.
enum class TextureType {
  TEXTURE_2D = 0,
  CUBEMAP,
};

inline const GLenum textureTypeToGlTarget(TextureType type) {
  switch (type) {
    case TextureType::TEXTURE_2D:
      return GL_TEXTURE_2D;
    case TextureType::CUBEMAP:
      return GL_TEXTURE_CUBE_MAP;
  }
  throw TextureException("ERROR::TEXTURE::INVALID_TEXTURE_TYPE\n" +
                         std::to_string(static_cast<int>(type)));
}

// The type of texture binding.
enum class TextureBindType {
  // By default we bind by texture type.
  BY_TEXTURE_TYPE = 0,
  // A TEXTURE_2D.
  TEXTURE_2D,
  // A cubemap.
  CUBEMAP,
  // An image texture that is directly indexed, rather than sampled.
  IMAGE_TEXTURE,
};

inline const TextureBindType textureTypeToTextureBindType(TextureType type) {
  switch (type) {
    case TextureType::TEXTURE_2D:
      return TextureBindType::TEXTURE_2D;
    case TextureType::CUBEMAP:
      return TextureBindType::CUBEMAP;
  }
  throw TextureException("ERROR::TEXTURE::INVALID_TEXTURE_TYPE\n" +
                         std::to_string(static_cast<int>(type)));
}

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
  CLAMP_TO_BORDER,
  // TODO: Add the others?
};

enum class MipGeneration {
  // Never generates or reserves mipmaps.
  NEVER = 0,
  // Generates mipmaps when loading, but not when creating empty textures.
  ON_LOAD,
  // Always attempts to generate mipmaps when possible.
  ALWAYS,
};

struct TextureParams {
  // OpenGL texture coordinates start at the bottom-right of the image, so we
  // flip vertically by default.
  bool flipVerticallyOnLoad = true;
  TextureFiltering filtering = TextureFiltering::NEAREST;
  TextureWrapMode wrapMode = TextureWrapMode::REPEAT;
  glm::vec4 borderColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
  MipGeneration generateMips = MipGeneration::ON_LOAD;
  // Maximum number of mips to allocate. If negative, no maximum is used.
  int maxNumMips = -1;
};

// Returns the number of mips for an image of a given width/height.
int calculateNumMips(int width, int height);

// Returns the next mip size given an initial size.
ImageSize calculateNextMip(const ImageSize& mipSize);

// Returns the calculated size for a mip level.
ImageSize calculateMipLevel(int mip0Width, int mip0Height, int level);

class Texture {
 public:
  // Loads a texture from a given path.
  // TODO: Consider putting this in a TextureLoader class.
  static Texture load(const char* path, bool isSRGB = true);
  static Texture load(const char* path, bool isSRGB,
                      const TextureParams& params);

  // Loads an HDR texture from the given path.
  static Texture loadHdr(const char* path);
  static Texture loadHdr(const char* path, const TextureParams& params);

  // Loads a cubemap from a set of 6 textures for the faces. Textures must be
  // passed in order starting with GL_TEXTURE_CUBE_MAP_POSITIVE_X and
  // incrementing from there; namely, in the order right, left, top, bottom,
  // front, and back.
  static Texture loadCubemap(std::vector<std::string> faces);
  static Texture loadCubemap(std::vector<std::string> faces,
                             const TextureParams& params);

  // Creates a custom texture of the given size and format.
  static Texture create(int width, int height, GLenum internalFormat);
  static Texture create(int width, int height, GLenum internalFormat,
                        const TextureParams& params);
  static Texture createCubemap(int size, GLenum internalFormat);
  static Texture createCubemap(int size, GLenum internalFormat,
                               const TextureParams& params);
  // Creates a custom texture based on the given input data.
  // TODO: Change this to take an unsigned char ptr?
  static Texture createFromData(int width, int height, GLenum internalFormat,
                                const std::vector<glm::vec3>& data);
  static Texture createFromData(int width, int height, GLenum internalFormat,
                                const std::vector<glm::vec3>& data,
                                const TextureParams& params);

  // TODO: Replace this with proper RAII.
  void free();

  // Binds the texture to the given texture unit.
  // Unit should be a number starting from 0, not the actual texture unit's
  // GLenum. This will bind samplers normally, but will bind cubemaps as
  // cubemaps and custom textures as image textures.
  void bindToUnit(unsigned int textureUnit,
                  TextureBindType bindType = TextureBindType::BY_TEXTURE_TYPE);

  // Generates mipmaps for the current texture. Note that this will not succeed
  // for textures with immutable storage.
  void generateMips(int maxNumMips = -1);

  // Sets a min/max mip level allowed when sampling from this texture. This is
  // important to avoid undefined behavior when drawing to a mip level while
  // sampling from another.
  void setSamplerMipRange(int min, int max);
  // Resets the allowed mip range to default values.
  void unsetSamplerMipRange();

  unsigned int getId() const { return id_; }
  TextureType getType() const { return type_; }
  // Returns the path to a texture. Not applicable for cubemaps or generated
  // textures.
  std::string getPath() const { return path_; }
  int getWidth() const { return width_; }
  int getHeight() const { return height_; }
  int getNumChannels() const { return numChannels_; }
  int getNumMips() const { return numMips_; }
  // TODO: Remove GLenum from this API (use a custom enum).
  GLenum getInternalFormat() const { return internalFormat_; }

 private:
  // TODO: Texture lifetimes aren't managed currently, so they aren't unloaded.
  unsigned int id_;
  TextureType type_;
  std::string path_;
  int width_;
  int height_;
  int numChannels_;
  int numMips_;
  GLenum internalFormat_;

  // Applies the given params to the currently-active texture.
  static void applyParams(const TextureParams& params,
                          TextureType type = TextureType::TEXTURE_2D);

  friend class Framebuffer;
  friend class Attachment;
};

}  // namespace qrk

#endif
