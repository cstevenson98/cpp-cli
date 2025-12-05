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

inline long long maximum_joltage_line_width_2(const std::string &line) {
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

inline long long digits_to_decimal(const std::vector<int> &vector) {
  long long decimal = 0;
  for (int i = 0; i < vector.size(); i++) {
    decimal = decimal * 10 + vector[i];
  }
  return decimal;
}

struct FoundDigit {
  std::vector<int>::const_iterator pos;
  int digit;
};

// Finds the leftmost instance of the maximum in a vector<int>
inline FoundDigit leftmost_maximum(std::vector<int>::const_iterator start_iter,
                                   std::vector<int>::const_iterator end_iter) {
  auto max_iter = std::max_element(start_iter, end_iter);
  return FoundDigit{max_iter, *max_iter};
}

inline long long maximum_joltage(const std::string &line, int sum_width) {
  auto vector = line_to_vector(line);
  if (sum_width == 0) {
    return 0;
  }
  if (sum_width == 1) {
    return leftmost_maximum(vector.begin(), vector.end()).digit;
  }
  if (vector.size() <= sum_width) {
    return digits_to_decimal(vector);
  }

  // Take first
  std::vector<int> current_vector;

  // Initialize
  auto start_iter = vector.begin();
  auto end_iter = vector.end();

  // Find first digit
  auto found_digit = leftmost_maximum(start_iter, end_iter - (sum_width - 1));
  current_vector.push_back(found_digit.digit);

  // Find remaining digits in order
  for (int digit_number = 1; digit_number < sum_width; digit_number++) {
    found_digit = leftmost_maximum(found_digit.pos + 1,
                                   end_iter - (sum_width - 1 - digit_number));

    current_vector.push_back(found_digit.digit);
  }

  return digits_to_decimal(current_vector);
}
} // namespace maxjoltage
