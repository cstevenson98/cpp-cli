#include <gtest/gtest.h>

#include "printing.hpp"

namespace printing {
namespace {

TEST(PrintingTest, PadLinesTest) {
  auto lines = std::vector<std::string>{"@"};

  auto result = printing::pad_lines(lines);
  EXPECT_EQ(result.size(), 3);
  EXPECT_EQ(result[0], "...");
  EXPECT_EQ(result[1], ".@.");
  EXPECT_EQ(result[2], "...");
}

TEST(PrintingTest, LinesToGridTest) {
  auto lines = std::vector<std::string>{"@"};
  auto result = printing::lines_to_grid(lines);
  EXPECT_EQ(result.size(), 1);
  EXPECT_EQ(result[0].size(), 1);
  EXPECT_EQ(result[0][0], 1);
}

TEST(PrintingTest, LinesToGridTestGeneral) {
  auto lines = std::vector<std::string>{"@@@@@", "@@@@@", "@@@@@"};
  auto result = printing::lines_to_grid(lines);
  for (const auto &row : result) {
    for (const auto &cell : row) {
      EXPECT_EQ(cell, 1);
    }
  }
}

TEST(PrintingTest, HowManyLessThan4NeighborsTest) {
  // clang-format off
  auto lines = std::vector<std::string>{
    "@@@@@@@@@",
    "@@@@@@@@@",
    "@@@@@@@@@",
    "@@@@@@@@@",
    "@@@@@@@@@",
    "@@@@@@@@@",
    "@@@@@@@@@",
    "@@@@@@@@@",
    "@@@@@@@@@",
    "@@@@@@@@@",
  };
  // clang-format on

  auto result = printing::how_many_less_than_4_neighbors(lines);

  EXPECT_EQ(result, 4);
}

// ..@@.@@@@.
// @@@.@.@.@@
// @@@@@.@.@@
// @.@@@@..@.
// @@.@@@@.@@
// .@@@@@@@.@
// .@.@.@.@@@
// @.@@@.@@@@
// .@@@@@@@@.
// @.@.@@@.@.

// There are 13

// clang-format off
const auto kLines = std::vector<std::string>{
  "..@@.@@@@.",
  "@@@.@.@.@@",
  "@@@@@.@.@@",
  "@.@@@@..@.",
  "@@.@@@@.@@",
  ".@@@@@@@.@",
  ".@.@.@.@@@",
  "@.@@@.@@@@",
  ".@@@@@@@@.",
  "@.@.@@@.@.",
};
// clang-format on

TEST(PrintingTest, PrintGridTest2) {
  auto result = printing::how_many_less_than_4_neighbors(kLines);

  EXPECT_EQ(result, 13);
}

TEST(PrintingTest, TotalCanRemoveTest) {
  auto lines = kLines;
  auto result = printing::total_can_remove(lines, 20);
  EXPECT_EQ(result, 43);
}

// Tests for grid_convolve_padding_assumed validation
TEST(GridConvolveTest, EmptyGridReturnsEmpty) {
  Grid empty_grid = {};
  auto result = grid_convolve_padding_assumed(empty_grid, kNeighborsKernel);
  EXPECT_TRUE(result.empty());
}

TEST(GridConvolveTest, EmptyRowReturnsEmpty) {
  Grid grid_with_empty_row = {{}};
  auto result =
      grid_convolve_padding_assumed(grid_with_empty_row, kNeighborsKernel);
  EXPECT_TRUE(result.empty());
}

TEST(GridConvolveTest, EmptyKernelReturnsEmpty) {
  Grid grid = {{1, 1}, {1, 1}};
  Grid empty_kernel = {};
  auto result = grid_convolve_padding_assumed(grid, empty_kernel);
  EXPECT_TRUE(result.empty());
}

TEST(GridConvolveTest, EmptyKernelRowReturnsEmpty) {
  Grid grid = {{1, 1}, {1, 1}};
  Grid kernel_with_empty_row = {{}};
  auto result = grid_convolve_padding_assumed(grid, kernel_with_empty_row);
  EXPECT_TRUE(result.empty());
}

TEST(GridConvolveTest, GridSmallerThanKernelReturnsEmpty) {
  Grid small_grid = {{1, 1}, {1, 1}};
  auto result = grid_convolve_padding_assumed(small_grid, kNeighborsKernel);
  EXPECT_TRUE(result.empty());
}

TEST(GridConvolveTest, SimpleConvolution) {
  // clang-format off
  Grid padded_grid = {
    {-1, -1, -1, -1},
    {-1,  1,  1, -1},
    {-1,  1,  1, -1},
    {-1, -1, -1, -1},
  };
  Grid expected = {
    {-1, -1, -1, -1},
    {-1,  3,  3, -1},
    {-1,  3,  3, -1},
    {-1, -1, -1, -1},
  };
  // clang-format on

  auto result = grid_convolve_padding_assumed(padded_grid, kNeighborsKernel);

  ASSERT_EQ(result.size(), expected.size());
  for (size_t i = 0; i < result.size(); i++) {
    ASSERT_EQ(result[i].size(), expected[i].size());
    for (size_t j = 0; j < result[i].size(); j++) {
      EXPECT_EQ(result[i][j], expected[i][j])
          << "Mismatch at (" << i << ", " << j << ")";
    }
  }
}

}  // namespace
}  // namespace printing
