#pragma once

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <numeric>
#include <stdexcept>
#include <string>
#include <vector>

namespace tachyons {

class TachyonManifold {
public:
  enum class Cell : std::uint8_t {
    Dot,      // .
    Beam,     // |
    Start,    // S
    Splitter, // ^
  };

  using Row = std::vector<Cell>;
  using Manifold = std::vector<Row>;

private:
  std::vector<std::string> lines_;
  Manifold manifold_;

  using TimelinesCount = std::vector<long long>;

  // Each state process step should process one row of the manifold
  int rows_completed_{0};
  int beam_splits_count_{0};

  // When a beam splits, it creates two new timelines, so we need to keep track
  // of the number of timelines for each row.
  TimelinesCount timelines_count_;

  static char cell_to_char(Cell cell) {
    switch (cell) {
    case Cell::Dot:
      return '.';
    case Cell::Beam:
      return '|';
    case Cell::Start:
      return 'S';
    case Cell::Splitter:
      return '^';
    default:
      throw std::invalid_argument("Invalid cell to char conversion");
    }
  }

  static Cell char_to_cell(char c) {
    switch (c) {
    case '.':
      return Cell::Dot;
    case '|':
      return Cell::Beam;
    case 'S':
      return Cell::Start;
    case '^':
      return Cell::Splitter;
    default:
      throw std::invalid_argument("Invalid string to cell conversion");
    }
  }

  [[nodiscard]] Row string_to_row(const std::string &string) const {
    Row row;
    std::transform(
        string.begin(), string.end(), std::back_inserter(row),
        [this](std::string::value_type c) { return char_to_cell(c); });
    return row;
  }

  [[nodiscard]] Manifold
  lines_to_manifold(const std::vector<std::string> &lines) const {
    Manifold manifold;
    std::transform(
        lines.begin(), lines.end(), std::back_inserter(manifold),
        [this](const std::string &line) { return string_to_row(line); });
    return manifold;
  }

  [[nodiscard]] static std::string row_to_string(const Row &row) {
    std::string result;
    for (const auto &cell : row) {
      result += cell_to_char(cell);
    }
    return result;
  }

  [[nodiscard]] static std::string
  manifold_to_string(const Manifold &manifold) {
    std::string result;
    for (const auto &row : manifold) {
      result += row_to_string(row);
      result += '\n';
    }
    // Remove trailing newline if present
    if (!result.empty() && result.back() == '\n') {
      result.pop_back();
    }
    return result;
  }

public:
  TachyonManifold(const std::vector<std::string> &lines)
      : lines_(lines), manifold_(lines_to_manifold(lines)),
        timelines_count_(manifold_.empty() ? 0 : manifold_[0].size(), 0) {}
  ~TachyonManifold();

  [[nodiscard]] std::string get_current_row_as_string() const {
    std::string result;
    std::transform(manifold_[rows_completed_].begin(),
                   manifold_[rows_completed_].end(), std::back_inserter(result),
                   [this](Cell cell) { return cell_to_char(cell); });
    return result;
  }

  void update_manifold() {
    // Check if we can update (need a next row)
    if (rows_completed_ + 1 >= static_cast<int>(manifold_.size())) {
      return; // No more rows to update
    }

    auto next_timelines_count = timelines_count_;
    // Loop through the current row and update the next one
    for (std::size_t i = 0; i < manifold_[rows_completed_].size(); ++i) {
      switch (manifold_[rows_completed_][i]) {
      case Cell::Start:
        manifold_[rows_completed_ + 1][i] = Cell::Beam;
        // Add one to the timeline count of the cell created
        next_timelines_count[i] = 1;
        break;
      case Cell::Beam:
        // If meeting a splitter in next row, split into two beams
        if (manifold_[rows_completed_ + 1][i] == Cell::Splitter) {
          if (i > 0) {
            manifold_[rows_completed_ + 1][i - 1] = Cell::Beam;
            next_timelines_count[i - 1] +=
                timelines_count_[i] + timelines_count_[i - 1];
            next_timelines_count[i] = 0;
          }
          if (i < manifold_[rows_completed_].size() - 1) {
            manifold_[rows_completed_ + 1][i + 1] = Cell::Beam;
            next_timelines_count[i + 1] +=
                timelines_count_[i] + timelines_count_[i + 1];
            next_timelines_count[i] = 0;
          }
          beam_splits_count_++;
        } else {
          manifold_[rows_completed_ + 1][i] = Cell::Beam;
        }
        break;
      default:
        break;
      }
    }

    std::cerr << "next_timelines_count: ";
    for (const auto &count : next_timelines_count) {
      std::cerr << count << " ";
    }
    std::cerr << "\n";
    timelines_count_ = next_timelines_count;
    // Move to the next row
    rows_completed_++;
  }

  struct SolveResult {
    int beam_splits_count;
    long long total_timelines;
  };

  SolveResult solve() {
    while (has_more_rows()) {
      update_manifold();
    }
    return {beam_splits_count_, std::accumulate(timelines_count_.begin(),
                                                timelines_count_.end(), 0LL)};
  }

  [[nodiscard]] bool has_more_rows() const {
    return rows_completed_ < static_cast<int>(manifold_.size()) - 1;
  }

  [[nodiscard]] int get_rows_completed() const { return rows_completed_; }

  [[nodiscard]] int get_total_rows() const {
    return static_cast<int>(manifold_.size());
  }

  [[nodiscard]] std::vector<std::string> get_manifold_lines() const {
    std::vector<std::string> result;
    for (int i = 0;
         i <= rows_completed_ && i < static_cast<int>(manifold_.size()); ++i) {
      result.push_back(row_to_string(manifold_[i]));
    }
    return result;
  }

  [[nodiscard]] int get_beam_splits_count() const { return beam_splits_count_; }

  TachyonManifold(const TachyonManifold &) = delete;
  TachyonManifold &operator=(const TachyonManifold &) = delete;
  TachyonManifold(TachyonManifold &&) = delete;
  TachyonManifold &operator=(TachyonManifold &&) = delete;
};
} // namespace tachyons
