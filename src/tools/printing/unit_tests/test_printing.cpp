#include <gtest/gtest.h>

#include "printing.hpp"

namespace printing {
namespace {

TEST(PrintingTest, PrintGridTest) {
  Grid grid({"..@@.@@@@.", "@@@@.@.@.@@", "@@@@@.@.@@", "@.@@@@..@.",
             "@@.@@@@.@@", ".@@@@@@@.@", ".@.@.@.@@@", "@.@@@.@@@@",
             ".@@@@@@@@.", "@.@.@@@.@."});
  grid.print_occupied_grid();
}
}  // namespace
}  // namespace printing
