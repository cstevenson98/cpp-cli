#include <gtest/gtest.h>

#include "cephalopods.hpp"

namespace cephalopods {
namespace {

TEST(DoHomeworkTest, Add) {
  std::vector<std::string> lines = {
      "1 2 3",
      "4 5 6",
      "+ + +",
  };
  auto result = do_homework(lines);
  EXPECT_EQ(result.value(), 21);
}

TEST(DoHomeworkTest, Multiply) {
  std::vector<std::string> lines = {
      "1 2 3",
      "4 5 6",
      "* * *",
  };
  auto result = do_homework(lines);
  EXPECT_EQ(result.value(), 32);
}

TEST(DoHomeworkTest, ExampleTest) {
  std::vector<std::string> lines = {
      "123 328  51 64",
      "45 64  387 23",
      "6 98  215 314",
      "*   +   *   +",
  };
  auto result = do_homework(lines);
  EXPECT_EQ(result.value(), 4277556);
}
} // namespace
} // namespace cephalopods
