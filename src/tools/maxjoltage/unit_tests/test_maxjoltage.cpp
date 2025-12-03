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

// insert_into_sorted_list tests
TEST(InsertIntoSortedListTest, InsertIntoEmpty) {
  std::list<int> list;
  insert_into_sorted_list(list, 5, 3);
  ASSERT_EQ(list.size(), 1u);
  EXPECT_EQ(list.front(), 5);
}

TEST(InsertIntoSortedListTest, MaintainsSortedOrder) {
  std::list<int> list;
  insert_into_sorted_list(list, 3, 5);
  insert_into_sorted_list(list, 1, 5);
  insert_into_sorted_list(list, 2, 5);

  auto it = list.begin();
  EXPECT_EQ(*it++, 1);
  EXPECT_EQ(*it++, 2);
  EXPECT_EQ(*it++, 3);
}

TEST(InsertIntoSortedListTest, TruncatesToSumWidth) {
  std::list<int> list;
  insert_into_sorted_list(list, 1, 2);
  insert_into_sorted_list(list, 2, 2);
  insert_into_sorted_list(list, 3, 2);

  ASSERT_EQ(list.size(), 2u);
}

// list_sum tests
TEST(ListSumTest, EmptyList) {
  std::list<int> list;
  EXPECT_EQ(list_sum(list), 0);
}

TEST(ListSumTest, SingleElement) {
  std::list<int> list = {5};
  EXPECT_EQ(list_sum(list), 5);
}

TEST(ListSumTest, MultipleElements) {
  std::list<int> list = {1, 2, 3, 4};
  EXPECT_EQ(list_sum(list), 10);
}

// maximum_joltage_line tests
TEST(MaximumJoltageLineTest, EmptyLine) {
  auto result = maximum_joltage_line("");
  EXPECT_EQ(result, 0);
}

TEST(MaximumJoltageLineTest, SingleDigit) {
  auto result = maximum_joltage_line("5");
  EXPECT_EQ(result, 5);
}

TEST(MaximumJoltageLineTest, DefaultSumWidth) {
  auto result = maximum_joltage_line("12345");
  // TODO: Verify expected behavior
  EXPECT_EQ(result, 9);
}

TEST(MaximumJoltageLineTest, CustomSumWidth) {
  auto result = maximum_joltage_line("12345", 3);
  // TODO: Verify expected behavior
  EXPECT_EQ(result, 12);
}

TEST(MaximumJoltageLineTwoNines, CustomSumWidth) {
  auto result = maximum_joltage_line("9999999999");
  EXPECT_EQ(result, 18);
}

TEST(MaximumJoltageLineLongString, CustomSumWidth) {
  auto result = maximum_joltage_line(
      "873692477333777493844793944369834547485424659388147992545248474574646638"
      "2439946467949468336382974347");
  EXPECT_EQ(result, 18);
}

}  // namespace
}  // namespace maxjoltage
