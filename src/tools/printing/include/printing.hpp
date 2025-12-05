#pragma once

#include <algorithm>
#include <cstdio>
#include <map>
#include <string>
#include <vector>

/**
  Solution to Day 4 Advent of Code 2025
  https://adventofcode.com/2025/day/4


  The problem is to determine how many cells can be removed from a grid of
  cells, where each cell can be removed if it has less than 4 neighbors. The
  solution is to use a convolution kernel to determine which cells can be
  removed. The kernel is a 3x3 matrix of 1s and 0s, where the center is 0 and
  the rest are 1s. The convolution is performed on a padded grid, with the
  padding being -1s. The result is a grid of 0s and 1s, where the 0s are the
  cells that can be removed and the 1s are the cells that cannot be removed.

  This is repeated iteratively until no cells can be removed, with the tally
  of cells that can be removed being returned.
*/

namespace printing {

using GridRow = std::vector<int>;
using Grid = std::vector<GridRow>;
using StringGrid = std::vector<std::string>;

// clang-format off
const Grid kNeighborsKernel = 
  {
    {1, 1, 1},
    {1, 0, 1},
    {1, 1, 1},
  };
// clang-format on

// Default maximum number of iterations to run the total can remove algorithm
constexpr int kMaxIterations = 1000;

// Minimum number of neighbors required to keep a cell
constexpr int kMinNeighborsToKeep = 4;

/**
 * @brief Prints a numeric grid to stdout with alignment for negative numbers.
 *
 * @param grid The grid of integers to print.
 */
inline void print_grid(const Grid &grid) {
  for (const auto &row : grid) {
    for (const auto &cell : row) {
      std::printf(" ");
      if (cell >= 0) {
        std::printf(" ");
      }
      std::printf("%d", cell);
    }
    std::printf("\n");
  }
}

/**
 * @brief Prints a string grid to stdout, one line per row.
 *
 * @param lines The grid of strings to print.
 */
inline void print_grid(const StringGrid &lines) {
  for (const auto &line : lines) {
    std::printf("%s\n", line.c_str());
  }
}

/**
 * @brief Pads a string grid with '.' characters on all sides.
 *
 * Adds one row of dots above and below, and one '.' on each side of every line.
 *
 * @param lines The input string grid (must be non-empty).
 * @return A new grid with padding applied.
 */
[[nodiscard]] inline StringGrid pad_lines(const StringGrid &lines) {
  StringGrid padded_lines;
  padded_lines.reserve(lines.size() + 2);

  // First line is all "."
  padded_lines.emplace_back(lines[0].size() + 2, '.');

  // Middle lines are the same as the input lines, with "." on the sides
  for (const auto &line : lines) {
    padded_lines.emplace_back("." + line + ".");
  }

  // Last line is all "."
  padded_lines.emplace_back(lines[0].size() + 2, '.');
  return padded_lines;
}

/**
 * @brief Converts a string line to a binary row.
 *
 * @param line The input string.
 * @return A GridRow where '@' maps to 1 and all other characters map to 0.
 */
[[nodiscard]] inline GridRow line_to_row(const std::string &line) {
  GridRow row;
  row.reserve(line.size());

  std::transform(line.begin(), line.end(), std::back_inserter(row),
                 [](char c) { return c == '@' ? 1 : 0; });
  return row;
}

/**
 * @brief Converts a string grid to a binary numeric grid.
 *
 * @param lines The input string grid.
 * @return A Grid where '@' maps to 1 and all other characters map to 0.
 */
[[nodiscard]] inline Grid lines_to_grid(const StringGrid &lines) {
  Grid grid;
  grid.reserve(lines.size());
  std::transform(lines.begin(), lines.end(), std::back_inserter(grid),
                 line_to_row);
  return grid;
}

/**
 * @brief Computes convolution at a single cell, treating negative values as 0.
 *
 * @param padded_grid The input grid with padding.
 * @param i Row index of the center cell.
 * @param j Column index of the center cell.
 * @param kernel The convolution kernel (typically 3x3).
 * @return The sum of element-wise products, with negatives clipped to 0.
 */
[[nodiscard]] inline int convolve_cell_clipped(const Grid &padded_grid, int i,
                                               int j, const Grid &kernel) {
  int sum = 0;
  for (std::size_t k = 0; k < kernel.size(); ++k) {
    for (std::size_t l = 0; l < kernel[k].size(); ++l) {
      sum += std::max(0, padded_grid[i + k - 1][j + l - 1]) * kernel[k][l];
    }
  }
  return sum;
}

/**
 * @brief Checks if a grid is valid (non-empty with non-empty first row).
 *
 * @param grid The grid to validate.
 * @return True if the grid has at least one row with at least one column.
 */
[[nodiscard]] inline bool is_valid_grid(const Grid &grid) {
  return !grid.empty() && !grid[0].empty();
}

/**
 * @brief Convolves an entire grid with a kernel, assuming input is pre-padded.
 *
 * Cells with value 0 in the input are skipped and output as -1.
 * The output maintains the same dimensions as the input, with -1 padding.
 *
 * @param padded_grid The input grid (must already have 1-cell padding).
 * @param kernel The convolution kernel.
 * @return The convolved grid, or empty grid if inputs are invalid.
 */
[[nodiscard]] inline Grid grid_convolve_padding_assumed(const Grid &padded_grid,
                                                        const Grid &kernel) {
  if (!is_valid_grid(padded_grid) || !is_valid_grid(kernel)) {
    return {};
  }
  if (padded_grid.size() < kernel.size() ||
      padded_grid[0].size() < kernel[0].size()) {
    return {};
  }

  Grid padded_result;
  padded_result.reserve(padded_grid.size());

  // Keep padding, with -1s
  padded_result.emplace_back(padded_grid[0].size(), -1);

  // Iterate over the grid, skipping the first and last row and column
  for (std::size_t i = 1; i < padded_grid.size() - 1; ++i) {
    GridRow row;
    row.reserve(padded_grid[i].size());
    row.emplace_back(-1);
    for (std::size_t j = 1; j < padded_grid[i].size() - 1; ++j) {
      if (padded_grid[i][j] == 0) {
        row.emplace_back(-1);
        continue;
      }

      int sum = convolve_cell_clipped(padded_grid, static_cast<int>(i),
                                      static_cast<int>(j), kernel);
      row.emplace_back(sum);
    }
    row.emplace_back(-1);
    padded_result.push_back(std::move(row));
  }
  padded_result.emplace_back(padded_grid[0].size(), -1);
  return padded_result;
}

// A map of coordinates and the number of cells that can be removed from them
using Coord = std::pair<int, int>;
using CanRemoveMap = std::map<Coord, int>;

/**
 * @brief Determines if a cell can be removed based on its neighbor count.
 *
 * @param value The cell's neighbor count.
 * @return True if value is in range [0, kMinNeighborsToKeep).
 */
[[nodiscard]] inline bool can_remove(int value) {
  return value >= 0 && value < kMinNeighborsToKeep;
}

/**
 * @brief Prints a map of removable cell coordinates and their neighbor counts.
 *
 * @param can_remove_map Map of (row, col) coordinates to neighbor counts.
 */
inline void print_can_remove_map(const CanRemoveMap &can_remove_map) {
  for (const auto &[coord, count] : can_remove_map) {
    std::printf("(%d,%d): %d\n", coord.first, coord.second, count);
  }
}

/**
 * @brief Builds a map of all cells that can be removed from the grid.
 *
 * @param grid The grid to scan for removable cells.
 * @return Map of (row, col) coordinates to their neighbor counts for all
 *         cells where can_remove() returns true.
 */
[[nodiscard]] inline CanRemoveMap build_can_remove_map(const Grid &grid) {
  CanRemoveMap can_remove_map = {};
  for (std::size_t i = 0; i < grid.size(); ++i) {
    for (std::size_t j = 0; j < grid[i].size(); ++j) {
      if (can_remove(grid[i][j])) {
        can_remove_map[{static_cast<int>(i), static_cast<int>(j)}] = grid[i][j];
      }
    }
  }
  return can_remove_map;
}

/**
 * @brief Removes all cells in the map from the grid and rebuilds the map.
 *
 * For each cell to remove, sets it to -1 and decrements all neighbors.
 * Then rebuilds can_remove_map to reflect the new grid state.
 *
 * @param[in,out] grid The grid to modify.
 * @param[in,out] can_remove_map Map of cells to remove; rebuilt after removal.
 */
inline void remove_and_update_map(Grid &grid, CanRemoveMap &can_remove_map) {
  // update the grid to remove the cells in the can_remove_map
  for (const auto &[coord, count] : can_remove_map) {
    // remove the cell and all cells around it, set to -1, but don't let
    // neighbors drop below 0
    for (int i = coord.first - 1; i <= coord.first + 1; ++i) {
      for (int j = coord.second - 1; j <= coord.second + 1; ++j) {
        if ((i == coord.first && j == coord.second) || grid[i][j] < 0) {
          continue;
        }
        grid[i][j] = std::max(-1, grid[i][j] - 1);
      }
    }
    grid[coord.first][coord.second] = -1;
  }
  
  // Rebuild the can_remove_map
  can_remove_map = build_can_remove_map(grid);
}

/**
 * @brief Computes the total number of cells that can be iteratively removed.
 *
 * Algorithm:
 * 1. Pad the grid and compute neighbor counts via convolution.
 * 2. Remove all cells with fewer than kMinNeighborsToKeep neighbors.
 * 3. Update neighbor counts and repeat until no cells can be removed.
 *
 * @param lines The input string grid (where '@' represents a cell).
 * @param max_iterations Maximum iterations before stopping (default: 1000).
 * @param verbose If true, prints debug info to stderr after each iteration.
 * @return Total count of all cells removed across all iterations.
 */
[[nodiscard]] inline long long total_can_remove(
    const StringGrid &lines, int max_iterations = kMaxIterations,
    bool verbose = false) {
  const auto padded_lines = pad_lines(lines);
  const auto padded_grid = lines_to_grid(padded_lines);
  auto convolved_grid =
      grid_convolve_padding_assumed(padded_grid, kNeighborsKernel);

  auto can_remove_map = build_can_remove_map(convolved_grid);

  if (verbose) {
    std::fprintf(stderr, "=== Initial state ===\n");
    print_grid(convolved_grid);
    std::fprintf(stderr, "\nCan remove map:\n");
    print_can_remove_map(can_remove_map);
  }

  long long total = static_cast<long long>(can_remove_map.size());
  int iterations = 0;
  while (!can_remove_map.empty() && iterations < max_iterations) {
    remove_and_update_map(convolved_grid, can_remove_map);
    total += static_cast<long long>(can_remove_map.size());
    ++iterations;

    if (verbose) {
      std::fprintf(stderr, "\n=== Iteration %d ===\n", iterations);
      print_grid(convolved_grid);
      std::fprintf(stderr, "\nCan remove map:\n");
      print_can_remove_map(can_remove_map);
    }
  }

  if (iterations >= max_iterations) {
    std::fprintf(stderr, "Warning: Max iterations reached\n");
  }

  return total;
}

}  // namespace printing