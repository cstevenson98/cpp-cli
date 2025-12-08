#pragma once

#include <cmath>
#include <numeric>
#include <optional>
#include <sstream>
#include <stack>
#include <string>
#include <vector>

namespace cephalopods {

struct Homework {
  enum class Type {
    True,
    Naive,
  };

  enum class HomeworkOperator {
    Add,
    Multiply,
  };

  friend std::istream &operator>>(std::istream &is, HomeworkOperator &op) {
    char c;
    if (is >> c) {
      switch (c) {
      case '+':
        op = HomeworkOperator::Add;
        break;
      case '*':
        op = HomeworkOperator::Multiply;
        break;
      default:
        is.setstate(std::ios::failbit);
        break;
      }
    }
    return is;
  }

  using InputRow = std::vector<long long>;
  using Inputs = std::vector<InputRow>;
  using Operators = std::vector<HomeworkOperator>;
  using OperatorsWithWidths = std::vector<std::pair<HomeworkOperator, int>>;

  Inputs inputs_;
  Operators operators_;
  OperatorsWithWidths operators_with_positions_;

  // Take a line and get all integers surrounded by arbitraty sized spacing
  inline InputRow get_ints_from_line(const std::string &line) {
    std::vector<long long> result;
    std::istringstream iss(line);
    long long num;
    while (iss >> num) {
      result.emplace_back(num);
    }
    return result;
  }

  // Take a line and replace every character with an integer, or 0 for
  // whitespace
  static inline InputRow get_ints_from_line_true(const std::string &line) {
    InputRow result;
    for (const auto &c : line) {
      if (std::isdigit(c)) {
        result.push_back(std::stoi(std::string(1, c)));
      } else {
        result.push_back(0);
      }
    }
    return result;
  }

  static Operators get_operators_from_line(const std::string &line) {
    Operators result;
    std::istringstream iss(line);
    HomeworkOperator op;
    while (iss >> op) {
      result.emplace_back(op);
    }
    return result;
  }

  static OperatorsWithWidths
  get_operators_with_positions(const std::string &line) {
    OperatorsWithWidths result;

    std::istringstream iss(line);

    char c;
    int position = 0;
    while (iss >> c) {
      HomeworkOperator op;
      switch (c) {
      case '+':
        op = HomeworkOperator::Add;
        break;
      case '*':
        op = HomeworkOperator::Multiply;
        break;
      default:
        continue;
      }

      // -1 because we want the position of the character before the current one
      position = static_cast<int>(iss.tellg()) - 1;
      result.emplace_back(op, position);
    }

    return result;
  }

  /** now the input are in grids delimited by operators, with
      digits actually being read top to bottom in the same column */
  static Inputs get_inputs_true(const std::vector<std::string> &lines) {
    Inputs result;
    for (int i = 0; i < lines.size() - 1; ++i) {
      // don't ignore whitespacex
      auto row = Homework::get_ints_from_line_true(lines[i]);
      result.push_back(row);
    }
    return result;
  }

  Homework(std::vector<std::string> lines, Type type = Type::Naive) {
    switch (type) {
    case Type::True:
      operators_with_positions_ = get_operators_with_positions(lines.back());
      inputs_ = get_inputs_true(lines);
      break;
    case Type::Naive:
      for (int i = 0; i < lines.size() - 1; ++i) {
        inputs_.emplace_back(get_ints_from_line(lines[i]));
      }
      operators_ = get_operators_from_line(lines.back());
      break;
    }
  }

  long long AddColumn(const Inputs &inputs, int column) const {
    return std::accumulate(
        inputs.begin(), inputs.end(), 0LL,
        [column](long long sum, const auto &row) { return sum + row[column]; });
  }

  long long MultiplyColumn(const Inputs &inputs, int column) const {
    return std::accumulate(inputs.begin(), inputs.end(), 1LL,
                           [column](long long product, const auto &row) {
                             return product * row[column];
                           });
  }

  long long solve() const {
    long long result = 0;
    for (int i = 0; i < operators_.size(); ++i) {
      const auto &op = operators_[i];

      switch (op) {
      case HomeworkOperator::Add:
        result += AddColumn(inputs_, i);
        break;
      case HomeworkOperator::Multiply:
        result += MultiplyColumn(inputs_, i);
      }
    }
    return result;
  }

  long long get_column(int column) const {
    std::stack<long long> stack;
    for (int i = 0; i < inputs_.size(); ++i) {
      if (inputs_[i][column] != 0) {
        stack.push(inputs_[i][column]);
      }
    }

    long long result = 0;
    long long iteration = 0;
    while (!stack.empty()) {
      result += stack.top() * std::pow(10LL, iteration);
      stack.pop();
      ++iteration;
    }

    return result;
  }

  std::vector<long long> get_problem_inputs(int position, int width) const {
    std::vector<long long> result;
    for (int i = position; i < position + width; ++i) {
      result.push_back(get_column(i));
    }
    return result;
  }

  long long solve_true(bool verbose = false) const {
    long long result = 0;
    for (int i = 0; i < operators_with_positions_.size(); ++i) {
      const auto &op = operators_with_positions_[i].first;
      const auto &position = operators_with_positions_[i].second;

      // Get the columns of this problem, which must be interpreted top
      // to bottom, while ignoring the 0s. Use a stack to grab them
      // the top of the stack, and add them to the result.
      auto width = i + 1 < operators_with_positions_.size()
                       ? operators_with_positions_[i + 1].second - position
                       : inputs_[0].size() - position + 1;
      auto problem_inputs = get_problem_inputs(position, width - 1);
      switch (op) {
      case HomeworkOperator::Add:
        if (verbose) {
          std::printf("performing add on ");
          for (const auto &input : problem_inputs) {
            std::printf("%lld ", input);
          }
          std::printf("\n");
        }

        result +=
            std::accumulate(problem_inputs.begin(), problem_inputs.end(), 0LL);
        break;
      case HomeworkOperator::Multiply:
        if (verbose) {
          std::printf("performing multiply on ");
          for (const auto &input : problem_inputs) {
            std::printf("%lld ", input);
          }
          std::printf("\n");
        }

        result +=
            std::transform_reduce(problem_inputs.begin(), problem_inputs.end(),
                                  1LL, std::multiplies<long long>(),
                                  [](long long input) { return input; });
      }
    }
    return result;
  }
};

/// Process homework lines
inline std::optional<long long>
do_homework(const std::vector<std::string> &lines, bool verbose = false) {
  if (lines.empty()) {
    return std::nullopt;
  }

  try {
    Homework homework(lines);
    const auto result = homework.solve();
    return std::make_optional(result);
  } catch (const std::exception &e) {
    std::fprintf(stderr, "Error: %s\n", e.what());
    return std::nullopt;
  }
}

inline std::optional<long long>
do_homework_true(const std::vector<std::string> &lines, bool verbose = false) {
  if (lines.empty()) {
    return std::nullopt;
  }

  try {
    Homework homework(lines, Homework::Type::True);
    const auto result = homework.solve_true(verbose);
    return std::make_optional(result);
  } catch (const std::exception &e) {
    std::fprintf(stderr, "Error: %s\n", e.what());
    return std::nullopt;
  }
}

} // namespace cephalopods
