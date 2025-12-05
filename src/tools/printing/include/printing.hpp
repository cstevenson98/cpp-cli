#pragma once

#include <algorithm>
#include <map>
#include <string>
#include <vector>

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

const int kMaxIterations = 1000;

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

inline void print_grid(const StringGrid &lines) {
  for (const auto &line : lines) {
    std::printf("%s\n", line.c_str());
  }
}

inline StringGrid pad_lines(const StringGrid &lines) {
  StringGrid padded_lines;

  // First line is all "."
  padded_lines.push_back(std::string(lines[0].size() + 2, '.'));

  // Middle lines are the same as the input lines, with "." on the sides
  for (const auto &line : lines) {
    padded_lines.push_back("." + line + ".");
  }

  // Last line is all "."
  padded_lines.push_back(std::string(lines[0].size() + 2, '.'));
  return padded_lines;
}

inline Grid lines_to_grid(const StringGrid &lines) {
  Grid grid;
  for (const auto &line : lines) {
    GridRow row;
    for (const auto &c : line) {
      row.push_back(c == '@' ? 1 : 0);
    }
    grid.push_back(row);
  }
  return grid;
}

inline Grid grid_convolve_padding_assumed(const Grid &padded_grid,
                                          const Grid &kernel) {
  if (padded_grid.empty()) {
    return Grid();
  }
  if (padded_grid[0].empty()) {
    return Grid();
  }

  if (kernel.empty()) {
    return Grid();
  }
  if (kernel[0].empty()) {
    return Grid();
  }
  if (padded_grid.size() < kernel.size() ||
      padded_grid[0].size() < kernel[0].size()) {
    return Grid();
  }

  Grid padded_result;

  // Keep padding, with -1s
  padded_result.push_back(GridRow(padded_grid[0].size(), -1));

  // Iterate over the grid, skipping the first and last row and column
  for (int i = 1; i < padded_grid.size() - 1; i++) {
    GridRow row;
    row.push_back(-1);
    for (int j = 1; j < padded_grid[i].size() - 1; j++) {
      int sum = 0;

      if (padded_grid[i][j] == 0) {
        row.push_back(-1);
        continue;
      }

      for (int k = 0; k < kernel.size(); k++) {
        for (int l = 0; l < kernel[k].size(); l++) {
          sum += padded_grid[i + k - 1][j + l - 1] * kernel[k][l];
        }
      }
      row.push_back(sum);
    }
    row.push_back(-1);
    padded_result.push_back(row);
  }
  padded_result.push_back(GridRow(padded_grid[0].size(), -1));
  return padded_result;
}

using Coord = std::pair<int, int>;
using CanRemoveMap = std::map<Coord, int>;

inline bool can_remove(int value) { return value >= 0 && value < 4; }

inline void print_can_remove_map(const CanRemoveMap &can_remove_map) {
  for (const auto &[coord, count] : can_remove_map) {
    std::printf("(%d,%d): %d\n", coord.first, coord.second, count);
  }
}

inline CanRemoveMap build_can_remove_map(const Grid &grid) {
  CanRemoveMap can_remove_map = {};
  for (int i = 0; i < grid.size(); i++) {
    for (int j = 0; j < grid[i].size(); j++) {
      if (can_remove(grid[i][j])) {
        can_remove_map[{i, j}] = grid[i][j];
      }
    }
  }
  return can_remove_map;
}

inline void remove_and_update_map(Grid &grid, CanRemoveMap &can_remove_map) {
  // update the grid to remove the cells in the can_remove_map
  for (const auto &[coord, count] : can_remove_map) {
    // remove the cell and all cells around it, set to -1, but don't let
    // neighbors drop below 0
    for (int i = coord.first - 1; i <= coord.first + 1; i++) {
      for (int j = coord.second - 1; j <= coord.second + 1; j++) {
        if (i == coord.first && j == coord.second || grid[i][j] < 0) {
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

// Determine how many total can be removed applying the following algorithm:
// 1. For each cell, if it has less than 4 neighbors, it can be removed.
// 2. If a cell can be removed, remove it and update the counts in all cells
// around it, logging any
//    cell that drop below 4
inline long long total_can_remove(const StringGrid &lines,
                                  int max_iterations = kMaxIterations,
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

  int total = can_remove_map.size();
  int iterations = 0;
  while (!can_remove_map.empty() && iterations < max_iterations) {
    remove_and_update_map(convolved_grid, can_remove_map);
    total += can_remove_map.size();
    iterations++;

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

inline int how_many_less_than_4_neighbors(const StringGrid &lines) {
  const auto padded_lines = pad_lines(lines);
  const auto padded_grid = lines_to_grid(padded_lines);
  const auto convolved_grid =
      grid_convolve_padding_assumed(padded_grid, kNeighborsKernel);

  int count = 0;
  for (const auto &row : convolved_grid) {
    for (const auto &cell : row) {
      if (can_remove(cell)) {
        count++;
      }
    }
  }
  return count;
}

}  // namespace printing