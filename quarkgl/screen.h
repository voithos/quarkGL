#ifndef QUARKGL_SCREEN_H_
#define QUARKGL_SCREEN_H_

#include <ostream>

namespace qrk {

struct ImageSize {
  int width;
  int height;
};

inline bool operator==(const ImageSize& lhs, const ImageSize& rhs) {
  return lhs.width == rhs.width && lhs.height == rhs.height;
}
inline bool operator!=(const ImageSize& lhs, const ImageSize& rhs) {
  return !operator==(lhs, rhs);
}
inline std::ostream& operator<<(std::ostream& os, const ImageSize& size) {
  return os << "(" << size.width << ", " << size.height << ")";
}

}  // namespace qrk

#endif