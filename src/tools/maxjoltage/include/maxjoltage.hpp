#pragma once

#include <algorithm>
#include <list>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>
namespace maxjoltage {

inline std::vector<int> line_to_vector(const std::string &line) {
  auto output = std::vector<int>();
  for (const auto &c : line) {
    output.push_back(std::stoi(std::string(1, c)));
  }
  return output;
}

inline int maximum_joltage_line_width_2(const std::string &line) {
  if (line.empty()) {
    return 0;
  }
  if (line.size() == 1) {
    return std::stoi(line);
  }

  // Get the vector of digits
  auto vector = line_to_vector(line);

  // Find the maximum of the list not including the end
  auto max = std::max_element(vector.begin(), vector.end() - 1);
  printf("max: %d\n", *max);
  // Find the leftmost location of the max not including the end
  auto leftmost_max_location =
      std::find(vector.begin(), vector.end() - 1, *max);

  printf("leftmost_max_location: %d\n", *leftmost_max_location);
  // Find max from this location to end
  auto max_from_leftmost_max_location =
      std::max_element(leftmost_max_location + 1, vector.end());
  printf("max_from_leftmost_max_location: %d\n",
         *max_from_leftmost_max_location);
  // Return the sum of the max and the max from the leftmost max location
  return 10 * *max + *max_from_leftmost_max_location;
}
}  // namespace maxjoltage
