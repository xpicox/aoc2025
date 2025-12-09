#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <mdspan>
#include <ostream>
#include <print>
#include <span>
#include <string>
#include <valarray>
#include <vector>

const char ROLL_C = '@';
const char EMPTY_C = '.';

std::vector<std::array<int64_t, 2>> indeces(int64_t rows, int64_t columns,
                                            int64_t row, int64_t column) {
  std::vector<std::array<int64_t, 2>> indeces;
  const int64_t neighbour{1};
  auto col_max = std::min(column + neighbour, columns - 1);
  auto col_min = std::max(column - neighbour, 0ll);
  auto row_max = std::min(row + neighbour, rows - 1);
  auto row_min = std::max(row - neighbour, 0ll);

  for (auto i = row_min; i <= row_max; ++i) {
    for (auto j = col_min; j <= col_max; ++j) {
      if (i == row && j == column) {
        continue;
      }
      indeces.push_back({i, j});
    }
  }
  return indeces;
}

// char grid
using cgrid_t = std::mdspan<char, std::dextents<std::size_t, 2>>;

int64_t eligible_rolls_1(cgrid_t &grid) {
  const auto rows{grid.extent(0)};
  const auto columns{grid.extent(1)};
  std::vector<std::array<int64_t, 2>> eligible;
  for (auto row = 0; row < rows; ++row) {
    for (auto col = 0; col < columns; ++col) {
      int64_t adjacenct_rolls{};
      if (grid[row, col] == ROLL_C) {
        for (auto [h, k] : indeces(rows, columns, row, col)) {
          adjacenct_rolls += grid[h, k] == ROLL_C;
        }
        if (adjacenct_rolls < 4) {
          eligible.push_back({row, col});
        }
      }
    }
  }
  return std::ssize(eligible);
}


int64_t eligible_rolls_2(cgrid_t &grid) {
  const auto rows{grid.extent(0)};
  const auto columns{grid.extent(1)};
  int64_t result{0};

  std::vector<std::array<int64_t, 2>> eligible;
  do {
    eligible.clear();
    for (auto row = 0; row < rows; ++row) {
      for (auto col = 0; col < columns; ++col) {
        int64_t adjacenct_rolls{};
        if (grid[row, col] == ROLL_C) {
          for (auto [h, k] : indeces(rows, columns, row, col)) {
            adjacenct_rolls += grid[h, k] == ROLL_C;
          }
          if (adjacenct_rolls < 4) {
            eligible.push_back({row, col});
          }
        }
      }
    }
    result += eligible.size();
    for (auto [row, col] : eligible){
      grid[row,col] = EMPTY_C;
    }
  } while (!eligible.empty());
  return result;
}

int main() {
  std::println("Ciao, {}!", "Mondo");
  std::string paper_rolls;
  std::getline(std::cin, paper_rolls);
  const int64_t columns = paper_rolls.size();
  std::copy(std::istream_iterator<char>(std::cin),
            std::istream_iterator<char>(), std::back_inserter(paper_rolls));
  // std::valarray<bool> val_rolls(paper_rolls.size());
  // std::transform(paper_rolls.cbegin(), paper_rolls.cend(),
  //                std::begin(val_rolls), [](char c) { return c == ROLL_C; });
  const int64_t rows = paper_rolls.size() / columns;
  cgrid_t grid = std::mdspan(paper_rolls.data(), rows, columns);
  std::println("Solution part 1: {}", eligible_rolls_1(grid));
  std::println("Solution part 2: {}", eligible_rolls_2(grid));
}
