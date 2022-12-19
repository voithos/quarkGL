#ifndef QUARKGL_RANDOM_H_
#define QUARKGL_RANDOM_H_

#include <random>

namespace qrk {

// A random generator for generating pseudorandom seeds.
class SeedGenerator {
 public:
  explicit SeedGenerator(unsigned int seed) : gen_(seed) {}

  // Returns the next sampled seed.
  unsigned int next() { return gen_(); }

  // Generates a single random seed using an entropy device.
  static unsigned int generateTrueRandomSeed();

 private:
  std::mt19937 gen_;
};

// A random distribution that generates uniform real numbers in the range
// [0.0, 1.0).
class UniformRandom {
 public:
  explicit UniformRandom(unsigned int seed)
      : gen_(seed), rand_(0.0f, 1.0f), seed_(seed) {}
  UniformRandom() : UniformRandom(SeedGenerator::generateTrueRandomSeed()) {}

  // Returns the next sampled random number.
  float next() { return rand_(gen_); }

  // Returns the seed used to initialize this random sampler.
  unsigned int getSeed() const { return seed_; }

 private:
  // RNG for random sampling. We use the Mersenne Twister because it has
  // high-quality characteristics.
  std::mt19937 gen_;
  std::uniform_real_distribution<float> rand_;
  const unsigned int seed_;
};

}  // namespace qrk

#endif
