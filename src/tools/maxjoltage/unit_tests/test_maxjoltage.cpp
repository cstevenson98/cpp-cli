#include <gtest/gtest.h>
#include <iterator>

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
  EXPECT_EQ(result, 0LL);
}

TEST(MaximumJoltageLineTest, SingleDigit) {
  auto result = maximum_joltage_line_width_2("5");
  EXPECT_EQ(result, 5LL);
}

TEST(MaximumJoltageLineTest, DefaultSumWidth) {
  auto result = maximum_joltage_line_width_2("12345");
  // TODO: Verify expected behavior
  EXPECT_EQ(result, 45LL);
}

TEST(MaximumJoltageLineTwoNines, CustomSumWidth) {
  auto result = maximum_joltage_line_width_2("9999999999");
  EXPECT_EQ(result, 99LL);
}

TEST(MaximumJoltageLineLongString1, CustomSumWidth) {
  auto result = maximum_joltage_line_width_2(
      "873692477333777493844793944369834547485424659388147992545248474574646638"
      "2439946467949468336382974347");
  EXPECT_EQ(result, 99LL);
}

TEST(MaximumJoltageLineLongString2, CustomSumWidth) {
  auto result = maximum_joltage_line_width_2(
      "252664423221241224234222221512252272525222332143124243224322321342212222"
      "3455121333122221232322333222");
  EXPECT_EQ(result, 75LL);
}

TEST(DigitsToDecimalTest, SingleDigit) {
  auto result = digits_to_decimal({1});
  EXPECT_EQ(result, 1LL);
}

TEST(DigitsToDecimalTest, MultipleDigits) {
  auto result = digits_to_decimal({1, 2, 3});
  EXPECT_EQ(result, 123LL);
}

TEST(DigitsToDecimalTest, ManyDigits) {
  auto result = digits_to_decimal({1, 2, 3, 5, 2, 6, 8, 1, 5, 6});
  EXPECT_EQ(result, 1235268156LL);
}

TEST(DigitsToDecimalTest, TwelveDigits) {
  auto result = digits_to_decimal({9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9});
  EXPECT_EQ(result, 999999999999LL);
}

TEST(MaximumJoltageTest, LeftmostMaximumTest) {
  std::vector<int> vector = {1, 9, 3, 4, 5};
  auto result = leftmost_maximum(vector.cbegin(), vector.cend());
  EXPECT_EQ(result.digit, 9);
  EXPECT_EQ(std::distance(vector.cbegin(), result.pos), 1);
}

TEST(MaximumJoltageTest, LeftmostMaximumTest1Element) {
  std::vector<int> vector = {1};
  auto result = leftmost_maximum(vector.cbegin(), vector.cend());
  EXPECT_EQ(result.digit, 1);
  EXPECT_EQ(std::distance(vector.cbegin(), result.pos), 0);
}

TEST(MaximumJoltageTest, LeftmostMaximumTest2Elements) {
  std::vector<int> vector = {1, 2};
  auto result = leftmost_maximum(vector.cbegin(), vector.cend());
  EXPECT_EQ(result.digit, 2);
  EXPECT_EQ(std::distance(vector.cbegin(), result.pos), 1);
}

TEST(MaximumJoltageTest, MaximumJoltageTest1Element) {
  std::string line = "1";
  auto result = maximum_joltage(line, 1);
  EXPECT_EQ(result, 1LL);
}

TEST(MaximumJoltageTest, MaximumJoltageTest2Elements) {
  std::string line = "12";
  auto result = maximum_joltage(line, 2);

  EXPECT_EQ(result, 12LL);
}

TEST(MaximumJoltageTest, MaximumJoltageTest3Elements) {
  std::string line = "123";
  auto result = maximum_joltage(line, 2);
  EXPECT_EQ(result, 23LL);
}

TEST(MaximumJoltageTest, MaximumJoltageTest4Elements) {
  std::string line = "1234";
  auto result = maximum_joltage(line, 2);
  EXPECT_EQ(result, 34LL);
}

TEST(MaximumJoltageTest, MaximumJoltageTest5Elements) {
  std::string line = "15342";
  auto result = maximum_joltage(line, 2);
  EXPECT_EQ(result, 54LL);
}

TEST(MaximumJoltageTest, MaximumJoltageTest5ElementsThreeSum) {
  std::string line = "15342";
  auto result = maximum_joltage(line, 3);
  EXPECT_EQ(result, 542LL);
}

TEST(MaximumJoltageTest, MaximumJoltageTestWidth12) {
  std::string line = "999999999999";
  auto result = maximum_joltage(line, 12);
  EXPECT_EQ(result, 999999999999LL);
}

TEST(MaximumJoltageTest, MaximumJoltageTestWidth12WithExtraDigits) {
  std::string line = "12399999999999";
  auto result = maximum_joltage(line, 12);
  EXPECT_EQ(result, 399999999999LL);
}

TEST(MaximumJoltageTest, MaximumJoltageTestLargeWidth) {
  std::string line = "987654321012345";
  auto result = maximum_joltage(line, 10);
  EXPECT_EQ(result, 9876543345LL);
}

} // namespace
} // namespace maxjoltage
