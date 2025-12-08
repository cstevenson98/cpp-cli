#pragma once

#include <algorithm>
#include <stdexcept>
#include <string>
#include <vector>

namespace tachyons {

class TachyonManifold {
public:
  enum class Cell {
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

  using BeamLocation = int;
  // In a quantum timeline, each split is actually two possible timelines, we
  // introduce a new BeamLocation to represent the two possible timelines
  using ActiveTimelines = std::vector<BeamLocation>;

  // Each state process step should process one row of the manifold
  int rows_completed_;
  int beam_splits_count_;
  ActiveTimelines active_timelines_;

  char cell_to_char(Cell cell) const {
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

  Cell char_to_cell(char c) const {
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

  Row string_to_row(const std::string &string) const {
    Row row;
    std::transform(
        string.begin(), string.end(), std::back_inserter(row),
        [this](std::string::value_type c) { return char_to_cell(c); });
    return row;
  }

  Manifold lines_to_manifold(const std::vector<std::string> &lines) const {
    Manifold manifold;
    std::transform(
        lines.begin(), lines.end(), std::back_inserter(manifold),
        [this](const std::string &line) { return string_to_row(line); });
    return manifold;
  }

  std::string row_to_string(const Row &row) const {
    std::string result;
    for (const auto &cell : row) {
      result += cell_to_char(cell);
    }
    return result;
  }

  std::string manifold_to_string(const Manifold &manifold) const {
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
      : lines_(lines), manifold_(lines_to_manifold(lines)), rows_completed_(0) {
  }
  ~TachyonManifold();

  std::string get_current_row_as_string() const {
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

    // Loop through the current row and update the next one
    for (std::size_t i = 0; i < manifold_[rows_completed_].size(); ++i) {
      switch (manifold_[rows_completed_][i]) {
      case Cell::Start:
        manifold_[rows_completed_ + 1][i] = Cell::Beam;
        break;
      case Cell::Beam:
        // If meeting a splitter in next row, split into two beams
        if (manifold_[rows_completed_ + 1][i] == Cell::Splitter) {
          if (i > 0)
            manifold_[rows_completed_ + 1][i - 1] = Cell::Beam;
          if (i < manifold_[rows_completed_].size() - 1)
            manifold_[rows_completed_ + 1][i + 1] = Cell::Beam;
          beam_splits_count_++;
        } else {
          manifold_[rows_completed_ + 1][i] = Cell::Beam;
        }
        break;
      default:
        break;
      }
    }

    // Move to the next row
    rows_completed_++;
  }

  // In this function we don't update the rows, we just use locations directly
  void update_manifold_quantum() {
    if (rows_completed_ + 1 >= static_cast<int>(manifold_.size())) {
      return;
    }

    // For all rows,
    for (std::size_t i = 0; i < manifold_.size(); ++i) {
      for ()
    }

    // Move to the next row
    rows_completed_++;
  }

  int solve() {
    while (has_more_rows()) {
      update_manifold();
    }
    return beam_splits_count_;
  }

  bool has_more_rows() const {
    return rows_completed_ < static_cast<int>(manifold_.size()) - 1;
  }

  int get_rows_completed() const { return rows_completed_; }

  int get_total_rows() const { return static_cast<int>(manifold_.size()); }

  std::vector<std::string> get_manifold_lines() const {
    std::vector<std::string> result;
    for (int i = 0;
         i <= rows_completed_ && i < static_cast<int>(manifold_.size()); ++i) {
      result.push_back(row_to_string(manifold_[i]));
    }
    return result;
  }

  int get_beam_splits_count() const { return beam_splits_count_; }

  TachyonManifold(const TachyonManifold &) = delete;
  TachyonManifold &operator=(const TachyonManifold &) = delete;
  TachyonManifold(TachyonManifold &&) = delete;
  TachyonManifold &operator=(TachyonManifold &&) = delete;
};
} // namespace tachyons
