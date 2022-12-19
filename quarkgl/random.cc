#include <qrk/random.h>

namespace qrk {

unsigned int SeedGenerator::generateTrueRandomSeed() {
  std::random_device rd_;
  return rd_();
}

}  // namespace qrk
