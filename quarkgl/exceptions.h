#ifndef QUARKGL_EXCEPTIONS_H_
#define QUARKGL_EXCEPTIONS_H_

#include <stdexcept>

namespace qrk {

class QuarkException : public std::runtime_error {
  using std::runtime_error::runtime_error;
};

}  // namespace qrk

#endif