#include <gtest/gtest.h>

#include "tachyon_manifold.hpp"

namespace tachyons {
namespace {
// clang-format off
  const std::vector<std::string> k_lines = {
    ".......S.......", 
    "...............", 
    ".......^.......",
    "...............", 
    "......^.^......", 
    "...............",
    ".....^.^.^.....", 
    "...............", 
    "....^.^...^....",
    "...............", 
   "...^.^...^.^...", 
   "...............",
   "..^...^.....^..", 
   "...............", 
   ".^.^.^.^.^...^.",
   "...............",
};

// clang-format off
const std::vector<std::string> k_lines1 = {
    ".......S.......", 
    ".......1.......", 
    "......1^1......",
    "......1.1......", 
    ".....1^2^1.....", 
    ".....1.2.1.....",
    "....1^3^3^1....", 
    "....1.3.3.1....", 
    "...1^4^331^1...",
    "...1.4.331.1...", 
   "..1^5^434^2^1..", 
   "...............",
   "..^...^.....^..", 
   "...............", 
   ".^.^.^.^.^...^.",
   "...............",
};
// clang-format on

TEST(TachyonManifoldTest, BasicConstruction) {
  TachyonManifold manifold(k_lines);
  manifold.update_manifold();
  EXPECT_EQ(manifold.get_rows_completed(), 1);
  EXPECT_EQ(manifold.get_total_rows(), 16);
  EXPECT_TRUE(manifold.has_more_rows());
  manifold.update_manifold();
  EXPECT_EQ(manifold.get_rows_completed(), 2);
  EXPECT_EQ(manifold.get_total_rows(), 16);
  EXPECT_TRUE(manifold.has_more_rows());
  manifold.update_manifold();
  EXPECT_EQ(manifold.get_rows_completed(), 3);
  EXPECT_EQ(manifold.get_total_rows(), 16);
  EXPECT_TRUE(manifold.has_more_rows());
}

TEST(TachyonManifoldTest, Solve) {
  TachyonManifold manifold(k_lines);
  auto result = manifold.solve();
  EXPECT_EQ(result.beam_splits_count, 21);
  EXPECT_EQ(result.total_timelines, 40);
}

} // namespace
} // namespace tachyons
