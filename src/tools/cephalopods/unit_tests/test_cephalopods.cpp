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
      "123 328  51 64  ",
      "45  64  387 23  ",
      "6   98  215 314 ",
      "*   +   *   +   ",
  };
  auto result = do_homework(lines);
  EXPECT_EQ(result.value(), 4277556);
}

TEST(DoHomeworkTest, GetOperatorsWithPositionsTest) {
  std::string line = "+  *    +   ";
  auto operators_with_positions = Homework::get_operators_with_positions(line);
  EXPECT_EQ(operators_with_positions.size(), 3);
  EXPECT_EQ(operators_with_positions[0].first, Homework::HomeworkOperator::Add);
  EXPECT_EQ(operators_with_positions[0].second, 0);
  EXPECT_EQ(operators_with_positions[1].first,
            Homework::HomeworkOperator::Multiply);
  EXPECT_EQ(operators_with_positions[1].second, 3);
  EXPECT_EQ(operators_with_positions[2].first, Homework::HomeworkOperator::Add);
  EXPECT_EQ(operators_with_positions[2].second, 8);
}

TEST(DoHomeworkTest, GetInputsTrueTest) {
  auto lines = std::vector<std::string>{
      "123 328  51 64  ",
      "45  64  387 23  ",
      "6   98  215 314 ",
      "*   +   *   +   ",
  };

  auto operators_with_widths =
      Homework::get_operators_with_positions(lines.back());

  auto expected_inputs = Homework::Inputs{
      {1, 2, 3, 0, 3, 2, 8, 0, 0, 5, 1, 0, 6, 4, 0, 0},
      {4, 5, 0, 0, 6, 4, 0, 0, 3, 8, 7, 0, 2, 3, 0, 0},
      {6, 0, 0, 0, 9, 8, 0, 0, 2, 1, 5, 0, 3, 1, 4, 0},
  };

  auto inputs = Homework::get_inputs_true(lines);
  for (int i = 0; i < lines.size() - 1; ++i) {
    for (int j = 0; j < expected_inputs[i].size(); ++j) {
      EXPECT_EQ(expected_inputs[i][j], inputs[i][j]);
    }
  }
}

TEST(DoHomeworkTest, GetColumnTest) {
  auto lines = std::vector<std::string>{
      "123 328  51 64  ",
      "45  64  387 23  ",
      "6   98  215 314 ",
      "*   +   *   +   ",
  };
  Homework homework(lines, Homework::Type::True);

  EXPECT_EQ(homework.get_column(0), 146);
  EXPECT_EQ(homework.get_column(1), 25);
  EXPECT_EQ(homework.get_column(2), 3);
  EXPECT_EQ(homework.get_column(3), 0);
  EXPECT_EQ(homework.get_column(4), 369);
  EXPECT_EQ(homework.get_column(5), 248);
  EXPECT_EQ(homework.get_column(6), 8);
  EXPECT_EQ(homework.get_column(7), 0);
  EXPECT_EQ(homework.get_column(8), 32);
  EXPECT_EQ(homework.get_column(9), 581);
  EXPECT_EQ(homework.get_column(10), 175);
  EXPECT_EQ(homework.get_column(11), 0);
  EXPECT_EQ(homework.get_column(12), 623);
  EXPECT_EQ(homework.get_column(13), 431);
  EXPECT_EQ(homework.get_column(14), 4);
  EXPECT_EQ(homework.get_column(15), 0);
}

TEST(DoHomeworkTest, GetProblemInputsTest) {
  auto lines = std::vector<std::string>{
      "123 328  51 64  ",
      "45  64  387 23  ",
      "6   98  215 314 ",
      "*   +   *   +   ",
  };
  Homework homework(lines, Homework::Type::True);

  std::vector<std::vector<long long>> expected_inputs = {
      {146, 25, 3},
      {369, 248, 8},
      {32, 581, 175},
      {623, 431, 4},
  };

  auto problem_inputs = homework.get_problem_inputs(0, 3);
  EXPECT_EQ(problem_inputs, expected_inputs[0]);
  problem_inputs = homework.get_problem_inputs(4, 3);
  EXPECT_EQ(problem_inputs, expected_inputs[1]);
  problem_inputs = homework.get_problem_inputs(8, 3);
  EXPECT_EQ(problem_inputs, expected_inputs[2]);
  problem_inputs = homework.get_problem_inputs(12, 3);
  EXPECT_EQ(problem_inputs, expected_inputs[3]);
}

TEST(DoHomeworkTest, SolveTrueTest) {
  auto lines = std::vector<std::string>{
      "123 328  51 64  ",
      " 45 64  387 23  ",
      "  6 98  215 314 ",
      "*   +   *   +   ",
  };

  std::vector<std::vector<long long>> expected_inputs = {
      {1, 24, 356},
      {369, 248, 8},
      {32, 581, 175},
      {623, 431, 4},
  };

  long long expected_soln =
      1 * 24 * 356 + (369 + 248 + 8) + 32 * 581 * 175 + (623 + 431 + 4);

  Homework homework(lines, Homework::Type::True);
  EXPECT_EQ(homework.solve_true(), expected_soln);
}

} // namespace
} // namespace cephalopods
