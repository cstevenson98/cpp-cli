#include <gtest/gtest.h>

#include "safes.hpp"

namespace safes {
namespace {
TEST(GetModulusTest, ZeroDigits) {
  auto result = safe_modulus(0);
  EXPECT_THROW(Safe(0, 0), std::invalid_argument);
}

TEST(GetModulusTest, OneDigit) {
  auto result = safe_modulus(1);
  EXPECT_EQ(result, 10);
}

TEST(GetModulusTest, TwoDigits) {
  auto result = safe_modulus(2);
  EXPECT_EQ(result, 100);
}

TEST(GetModulusTest, ThreeDigits) {
  auto result = safe_modulus(3);
  EXPECT_EQ(result, 1000);
}

TEST(GetModulusTest, FourDigits) {
  auto result = safe_modulus(4);
  EXPECT_EQ(result, 10000);
}

TEST(SafeTest, TurnRight) {
  Safe safe;
  safe.turn_right(1);
  EXPECT_EQ(safe.get_position(), 1);
}

TEST(SafeTest, TurnLeft) {
  Safe safe;
  safe.turn_left(1);
  EXPECT_EQ(safe.get_position(), 99);
}

TEST(SafeTest, TurnRight99) {
  Safe safe(99);
  safe.turn_right(1);
  EXPECT_EQ(safe.get_position(), 0);
}

TEST(SafeTest, TurnRightModulus) {
  Safe safe;
  safe.turn_right(100);
  EXPECT_EQ(safe.get_position(), 0);
}

TEST(SafeTest, TurnRightCountZeros) {
  Safe safe;
  safe.turn_right(100);
  EXPECT_EQ(safe.turn_count_zeros(100), 1);
  EXPECT_EQ(safe.get_position(), 0);
}

TEST(SafeTest, TurnLeftCountZeros) {
  Safe safe;
  safe.turn_left(200);
  EXPECT_EQ(safe.turn_count_zeros(-200), 2);
  EXPECT_EQ(safe.get_position(), 0);
}

TEST(SafeTest, TurnRightRemainderGivesExtraZero) {
  Safe safe(99);
  EXPECT_EQ(safe.turn_count_zeros(110), 2);
  EXPECT_EQ(safe.get_position(), (99 + 110) % 100);
}

TEST(SafeTest, TurnLeftRemainderGivesExtraZero) {
  Safe safe(5);
  EXPECT_EQ(safe.turn_count_zeros(-20), 1);
  EXPECT_EQ(safe.get_position(), 85 % 100);
}

TEST(SafeTest, TurnLeftRemainderGivesExtraZero2) {
  Safe safe(5);
  EXPECT_EQ(safe.turn_count_zeros(-105), 2);
  EXPECT_EQ(safe.get_position(), (5 - 105) % 100);
}

TEST(SafeTest, ApplyInstructionCountZeros) {
  Safe safe;
  safe.apply_instruction(Safe::Instruction::from_string("R100"));
  EXPECT_EQ(safe.apply_instruction_count_zeros(
                Safe::Instruction::from_string("R100")),
            1);
  EXPECT_EQ(safe.get_position(), 0);
  EXPECT_EQ(safe.apply_instruction_count_zeros(
                Safe::Instruction::from_string("L100")),
            1);
  EXPECT_EQ(safe.get_position(), 0);
  EXPECT_EQ(safe.apply_instruction_count_zeros(
                Safe::Instruction::from_string("R100")),
            1);
  EXPECT_EQ(safe.get_position(), 0);
  EXPECT_EQ(safe.apply_instruction_count_zeros(
                Safe::Instruction::from_string("L100")),
            1);
  EXPECT_EQ(safe.get_position(), 0);
  EXPECT_EQ(safe.apply_instruction_count_zeros(
                Safe::Instruction::from_string("R100")),
            1);
  EXPECT_EQ(safe.get_position(), 0);
}
} // namespace
} // namespace safes
