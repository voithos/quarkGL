#include <gtest/gtest.h>
#include <qrk/texture.h>

namespace {

TEST(CalculateNumMipsTest, Small) {
  EXPECT_EQ(qrk::calculateNumMips(1, 1), 1);
  EXPECT_EQ(qrk::calculateNumMips(2, 2), 2);
  EXPECT_EQ(qrk::calculateNumMips(2, 1), 2);
  EXPECT_EQ(qrk::calculateNumMips(1, 2), 2);
}

TEST(CalculateNumMipsTest, Large) {
  EXPECT_EQ(qrk::calculateNumMips(256, 256), 9);
}

TEST(CalculateNextMipTest, Base) {
  qrk::ImageSize expected = {1, 1};
  EXPECT_EQ(qrk::calculateNextMip({1, 1}), expected);
}

TEST(CalculateNextMipTest, Small) {
  qrk::ImageSize expected = {1, 1};
  EXPECT_EQ(qrk::calculateNextMip({2, 2}), expected);
}

TEST(CalculateNextMipTest, Large) {
  qrk::ImageSize expected = {512, 512};
  EXPECT_EQ(qrk::calculateNextMip({1024, 1024}), expected);
}

TEST(CalculateNextMipTest, Uneven) {
  qrk::ImageSize expected = {128, 512};
  EXPECT_EQ(qrk::calculateNextMip({256, 1024}), expected);
}

TEST(CalculateNextMipTest, NonPowerOfTwo) {
  qrk::ImageSize expected = {42, 13};
  EXPECT_EQ(qrk::calculateNextMip({84, 26}), expected);
}

TEST(CalculateMipLevelTest, Base) {
  qrk::ImageSize expected = {64, 64};
  EXPECT_EQ(qrk::calculateMipLevel(64, 64, 0), expected);
}

TEST(CalculateMipLevelTest, Next) {
  qrk::ImageSize expected = {64, 64};
  EXPECT_EQ(qrk::calculateMipLevel(128, 128, 1), expected);
}

TEST(CalculateMipLevelTest, Far) {
  qrk::ImageSize expected = {64, 64};
  EXPECT_EQ(qrk::calculateMipLevel(512, 512, 3), expected);
}

}  // namespace