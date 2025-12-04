#include <gtest/gtest.h>

#include "maxjoltage.hpp"

namespace maxjoltage {
namespace {

// line_to_vector tests
TEST(LineToVectorTest, EmptyString) {
  auto result = line_to_vector("");
  EXPECT_TRUE(result.empty());
}

TEST(LineToVectorTest, SingleDigit) {
  auto result = line_to_vector("5");
  ASSERT_EQ(result.size(), 1u);
  EXPECT_EQ(result[0], 5);
}

TEST(LineToVectorTest, MultipleDigits) {
  auto result = line_to_vector("123");
  ASSERT_EQ(result.size(), 3u);
  EXPECT_EQ(result[0], 1);
  EXPECT_EQ(result[1], 2);
  EXPECT_EQ(result[2], 3);
}

// maximum_joltage_line tests
TEST(MaximumJoltageLineTest, EmptyLine) {
  auto result = maximum_joltage_line_width_2("");
  EXPECT_EQ(result, 0);
}

TEST(MaximumJoltageLineTest, SingleDigit) {
  auto result = maximum_joltage_line_width_2("5");
  EXPECT_EQ(result, 5);
}

TEST(MaximumJoltageLineTest, DefaultSumWidth) {
  auto result = maximum_joltage_line_width_2("12345");
  // TODO: Verify expected behavior
  EXPECT_EQ(result, 45);
}

TEST(MaximumJoltageLineTwoNines, CustomSumWidth) {
  auto result = maximum_joltage_line_width_2("9999999999");
  EXPECT_EQ(result, 99);
}

TEST(MaximumJoltageLineLongString1, CustomSumWidth) {
  auto result = maximum_joltage_line_width_2(
      "873692477333777493844793944369834547485424659388147992545248474574646638"
      "2439946467949468336382974347");
  EXPECT_EQ(result, 99);
}

TEST(MaximumJoltageLineLongString2, CustomSumWidth) {
  auto result = maximum_joltage_line_width_2(
      "252664423221241224234222221512252272525222332143124243224322321342212222"
      "3455121333122221232322333222");
  EXPECT_EQ(result, 75);
  
}

}  // namespace
}  // namespace maxjoltage
