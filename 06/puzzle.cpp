#include <algorithm>
#include <array>
#include <cassert>
#include <charconv>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <istream>
#include <iterator>
#include <mdspan>
#include <ostream>
#include <print>
#include <span>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

int main() {
  std::println("Ciao, {}!", "Mondo");
  // Part 1
  std::string line;
  std::istringstream sline;
  std::vector<int64_t> numbers;
  std::vector<char> operators;
  std::size_t n_operands{0};
  auto is_sum = [](auto c) { return c == '+'; };
  auto is_product = [](auto c) { return c == '*'; };
  auto is_operator = [&is_sum, &is_product](auto c) {
    return is_sum(c) || is_product(c);
  };
  std::string full_input; // Part 2
  // Parse input
  while (std::getline(std::cin, line) && !is_operator(line[0])) {
    ++n_operands;
    full_input.append(line);
    full_input.push_back(' ');
  }
  // Parse operands
  sline = std::istringstream(full_input);
  std::copy(std::istream_iterator<int64_t>(sline),
            std::istream_iterator<int64_t>(), std::back_inserter(numbers));
  // Parse operators
  full_input.append(line);
  full_input.push_back(' ');
  sline = std::istringstream(std::move(line));
  std::copy(std::istream_iterator<char>(sline), std::istream_iterator<char>(),
            std::back_inserter(operators));

  // Solution Part 1
  const auto n_problems = std::size(operators);
  assert(std::size(numbers) % n_problems == 0);
  assert(std::size(numbers) / n_problems == n_operands);
  const auto operands = std::mdspan(numbers.data(), n_operands, n_problems);
  int64_t result_1{0};
  for (auto col = 0uz; col < n_problems; ++col) {
    const bool sum = is_sum(operators[col]);
    int64_t solution = sum ? 0 : 1;
    for (auto row = 0uz; row < n_operands; ++row) {
      if (sum) {
        solution += operands[row, col];
      } else {
        assert(operators[col] == '*');
        solution *= operands[row, col];
      }
    }
    result_1 += solution;
  }

  // Solution Part 2
  const auto input_rows = n_operands + 1;
  assert(std::size(full_input) % input_rows == 0);
  const auto input_cols = std::size(full_input) / input_rows;
  const auto input_grid =
      std::mdspan(full_input.data(), input_rows, input_cols);

  std::string transposed_input;
  transposed_input.reserve(full_input.size());
  auto back_inserter = std::back_inserter(transposed_input);
  for (auto col = 1uz; col <= input_cols; ++col) {
    for (auto row = 0uz; row < input_rows; ++row) {
      back_inserter = input_grid[row, input_cols - col];
    }
  }

  const auto row_width = static_cast<int64_t>(input_rows);
  auto is_whitespace = [](auto c) { return c == ' '; };
  std::vector<int64_t> operands_v = {};
  operands_v.reserve(8);
  int64_t result_2{0};
  for (auto it = transposed_input.cbegin(); it < transposed_input.cend();
       std::advance(it, row_width)) {
    const auto view_end = std::next(it, row_width);
    const auto view_begin = std::find_if_not(it, view_end, is_whitespace);
    if (view_begin == view_end) {
      continue;
    }
    auto const row = std::string_view{view_begin, view_end};
    operands_v.push_back(std::strtol(row.data(), nullptr, 10));
    if (is_sum(row.back())) {
      result_2 += std::ranges::fold_left(operands_v, 0l, std::plus());
      operands_v.clear();
    } else if (is_product(row.back())) {
      result_2 += std::ranges::fold_left(operands_v, 1l, std::multiplies());
      operands_v.clear();
    }
  }
  std::println("Solution part 1: {}", result_1);
  std::println("Solution part 2: {}", result_2);
}
