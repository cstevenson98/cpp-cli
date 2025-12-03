#pragma once

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

inline void insert_into_sorted_list(std::list<int> &list, int value,
                                    const int sum_width) {
  auto it = list.begin();
  while (it != list.end() && *it < value) {
    it++;
  }
  list.insert(it, value);
  if (list.size() > sum_width) {
    list.pop_front();
  }
}

inline int list_sum(const std::list<int> &list) {
  return std::accumulate(list.begin(), list.end(), 0);
}

inline int maximum_joltage_line(const std::string &line,
                                const int sum_width = 2) {
  auto vector = line_to_vector(line);

  std::list<int> candidate_maximal_joltages;
  int summed_candidate_joltages = 0;
  for (size_t i = 0; i < vector.size(); i++) {
    insert_into_sorted_list(candidate_maximal_joltages, vector[i], sum_width);
  }
  summed_candidate_joltages = list_sum(candidate_maximal_joltages);
  return summed_candidate_joltages;
}
}  // namespace maxjoltage
