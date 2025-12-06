#pragma once

#include <cmath>
#include <stdexcept>
#include <string>
#include <vector>

/**
input:
L68
L30
R48
L5
R60
L55
L1
L99
R14
L82
*/

namespace safes {
using StringGrid = std::vector<std::string>;

// For n digits, we need to map 10^n to 0
inline int safe_modulus(int num_digits) { return std::pow(10, num_digits); }

struct Safe {
  using Position = int;
  Position position_;
  int num_digits_;
  int modulus_;

  enum class Direction {
    Left,
    Right,
  };

  struct Instruction {
    Direction direction;
    int steps;

    [[nodiscard]] static Instruction
    from_string(const std::string &instruction_str) {
      auto direction =
          instruction_str[0] == 'L' ? Direction::Left : Direction::Right;
      auto steps = std::stoi(instruction_str.substr(1));
      return Instruction{direction, steps};
    }
  };

  /**
   * @brief Constructor for Safe
   * @param initial_position The initial position of the safe
   * @param num_digits The number of digits in the safe
   * @throws std::invalid_argument if num_digits is less than 1
   */
  Safe(Position initial_position = 0, int num_digits = 2)
      : position_(initial_position), num_digits_(num_digits),
        modulus_(safe_modulus(num_digits)) {
    if (num_digits < 1) {
      throw std::invalid_argument("num_digits must be greater than 0");
    }
  }

  Position get_position() const { return position_; }
  int get_num_digits() const { return num_digits_; }
  int get_modulus() const { return modulus_; }

  void turn_right(int amount) {
    const auto new_position = (position_ + amount) % modulus_;
    position_ = new_position < 0 ? new_position + modulus_ : new_position;
  }
  void turn_left(int amount) {
    const auto new_position = (position_ - amount) % modulus_;
    position_ = new_position < 0 ? new_position + modulus_ : new_position;
  }

  int turn_count_zeros(int amount) {
    // Express abs(amount) = n * modulus_ + remainder

    const auto n = std::abs(amount) / modulus_;
    const auto remainder = std::abs(amount) % modulus_;

    auto zero_crossings = 0;
    if (amount > 0) {
      zero_crossings = n + ((position_ + remainder) >= modulus_ ? 1 : 0);
      turn_right(abs(amount));
    } else {
      zero_crossings =
          n + ((position_ - remainder) <= 0 ? 1 : 0) - (position_ == 0 ? 1 : 0);
      turn_left(abs(amount));
    }

    return zero_crossings;
  }

  void apply_instruction(const Instruction &instruction) {
    if (instruction.direction == Direction::Left) {
      turn_left(instruction.steps);
    } else {
      turn_right(instruction.steps);
    }
  }

  int apply_instruction_count_zeros(const Instruction &instruction) {
    if (instruction.direction == Direction::Left) {
      return turn_count_zeros(instruction.steps);
    } else {
      return turn_count_zeros(-instruction.steps);
    }
  }
};

using Positions = std::vector<int>;

/**
 * @brief Get all positions after applying each instruction, including initial.
 */
[[nodiscard]] inline Positions
get_positions(const StringGrid &instructions_strs, int initial_position = 0,
              int num_digits = 2) {
  Safe safe(initial_position, num_digits);
  Positions positions;

  // Include initial position
  positions.push_back(safe.get_position());

  for (const auto &instruction_str : instructions_strs) {
    auto instruction = Safe::Instruction::from_string(instruction_str);
    safe.apply_instruction(instruction);
    positions.push_back(safe.get_position());
  }

  return positions;
}

// Logs final position, with the number of crossings of 0
using PositionsZeroCrossings = std::vector<std::pair<Safe::Position, int>>;

[[nodiscard]] inline PositionsZeroCrossings
get_position_zero_crossings(const StringGrid &instructions_strs,
                            int initial_position = 0, int num_digits = 2) {

  Safe safe(initial_position, num_digits);
  PositionsZeroCrossings positions_zero_crossings;

  for (const auto &instruction_str : instructions_strs) {
    auto instruction = Safe::Instruction::from_string(instruction_str);

    const auto zero_crossings_count =
        safe.apply_instruction_count_zeros(instruction);

    positions_zero_crossings.push_back(
        std::make_pair(safe.get_position(), zero_crossings_count));
  }
  return positions_zero_crossings;
}

/**
 * @brief Count the number of 0s in the positions vector (the password).
 */
[[nodiscard]] inline long long count_zeros(const Positions &positions) {
  long long count = 0;
  for (const auto &pos : positions) {
    if (pos == 0) {
      ++count;
    }
  }
  return count;
}

/**
 * @brief Crack the safe by counting 0s in positions after applying
 * instructions.
 */
[[nodiscard]] inline long long crack(const StringGrid &instructions_strs,
                                     int initial_position = 0,
                                     int num_digits = 2) {
  auto positions =
      get_positions(instructions_strs, initial_position, num_digits);
  return count_zeros(positions);
}

/**
 * @brief Crack the safe using the true protocol: "method 0x434C49434B", which
 * means we need to count every crossing of 0, not just when the final position
 * is 0
 */
[[nodiscard]] inline long long crack_true(const StringGrid &instructions_strs,
                                          int initial_position = 0,
                                          int num_digits = 2) {
  auto positions_zero_crossings = get_position_zero_crossings(
      instructions_strs, initial_position, num_digits);
  long long count = 0;
  for (const auto &pos_zero_crossings : positions_zero_crossings) {
    count += pos_zero_crossings.second;
  }
  return count;
}
} // namespace safes
