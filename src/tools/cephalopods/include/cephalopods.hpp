#pragma once

#include <numeric>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

namespace cephalopods {

struct Homework {
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

  using Inputs = std::vector<std::vector<long long>>;
  using InputTrue = std::vector<std::vector<std::vector<long long>>>;
  using Operators = std::vector<HomeworkOperator>;
  using OperatorsWithWidths = std::vector<std::pair<Operators, int>>;

  Inputs inputs_;
  InputTrue inputs_true_;
  Operators operators_;
  OperatorsWithWidths operators_with_widths_;
  // Take a line and get all integers surrounded by arbitraty sized spacing
  inline std::vector<long long> get_ints_from_line(const std::string &line) {
    std::vector<long long> result;
    std::istringstream iss(line);
    long long num;
    while (iss >> num) {
      result.emplace_back(num);
    }
    return result;
  }

  Operators get_operators_from_line(const std::string &line) {
    Operators result;
    std::istringstream iss(line);
    HomeworkOperator op;
    while (iss >> op) {
      result.emplace_back(op);
    }
    return result;
  }

  // For a string which looks like , for example
  // "+  *    + -  "
  // should return:
  // {(+, 2), (*, 4), (+, 1), (-, 2)}
  int get_width_from_line(const std::string &line) {
    std::istringstream iss(line);
    int width;
    iss >> width;
    return width;
  }

  OperatorsWithWidths get_operators_with_widths(const std::string &line) {
    OperatorsWithWidths result;
    int width = 0;
    for (const auto &c : line) {
      if (c == ' ') {
        width++;
      }
    }
    result.emplace_back(get_operators_from_line(line), width);
    return result;
  }

  InputTrue get_inputs_true(const std::vector<std::string> &lines,
                            const OperatorsWithWidths &operators_with_widths_) {
    InputTrue result;
    for (int i = 0; i < lines.size() - 1; ++i) {
      result.emplace_back(get_ints_from_line(lines[i]));
    }
    return result;
  }

  enum class Type {
    True,
    Naive,
  };

  Homework(std::vector<std::string> lines, Type type = Type::Naive) {
    switch (type) {
    case Type::True:
      operators_with_widths_ = get_operators_with_widths(lines.back());
      inputs_true_ = get_inputs_true(lines, operators_with_widths_);
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

  long long solve_true() const {
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
do_homework_true(const std::vector<std::string> &lines) {
  if (lines.empty()) {
    return std::nullopt;
  }

  try {
    Homework homework(lines);
    const auto result = homework.solve_true();
    return std::make_optional(result);
  } catch (const std::exception &e) {
    std::fprintf(stderr, "Error: %s\n", e.what());
    return std::nullopt;
  }
}

} // namespace cephalopods
