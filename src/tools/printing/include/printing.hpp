#pragma once

#include <string>
#include <vector>

namespace printing {

// from an input of the form:
// ..@@.@@@@.
// @@@.@.@.@@
// @@@@@.@.@@
// @.@@@@..@.
// @@.@@@@.@@
// .@@@@@@@.@
// .@.@.@.@@@
// @.@@@.@@@@
// .@@@@@@@@.
// @.@.@@@.@.
// counts how many @ have 4 or less neighbors in a 3x3 grid

struct Grid {
  std::vector<std::string> lines;
  Grid(const std::vector<std::string> &lines);

  std::vector<std::vector<int>> occupied_grid; // 0 for empty, 1 for occupied
  void fill_occupied_grid_with_padding();
  void print_occupied_grid() const;
  int sum_occupied_neighbors(int i, int j) const;
};

inline void Grid::fill_occupied_grid_with_padding() {
  occupied_grid.resize(lines.size() + 2,
                       std::vector<int>(lines[0].size() + 2, 0));
  // Copy the lines into the occupied grid, with padding
  for (int i = 0; i < lines.size(); i++) {
    for (int j = 0; j < lines[i].size(); j++) {
      occupied_grid[i + 1][j + 1] = lines[i][j] == '@' ? 1 : 0;
    }
  }
}

inline void Grid::print_occupied_grid() const {
  for (int i = 0; i < occupied_grid.size(); i++) {
    for (int j = 0; j < occupied_grid[i].size(); j++) {
      std::printf("%d", occupied_grid[i][j]);
    }
    std::printf("\n");
  }
}

inline int Grid::sum_occupied_neighbors(int i, int j) const {
  return occupied_grid[i - 1][j - 1] + occupied_grid[i - 1][j] +
         occupied_grid[i - 1][j + 1] + occupied_grid[i][j - 1] +
         occupied_grid[i][j + 1] + occupied_grid[i + 1][j - 1] +
         occupied_grid[i + 1][j] + occupied_grid[i + 1][j + 1];
}

inline Grid::Grid(const std::vector<std::string> &lines)
    : lines(lines),
      occupied_grid(lines.size(), std::vector<int>(lines[0].size(), 0)) {
  fill_occupied_grid_with_padding();
}

} // namespace printing