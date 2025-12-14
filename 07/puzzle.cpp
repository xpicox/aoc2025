#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <iterator>
#include <mdspan>
#include <print>
#include <span>
#include <string>
#include <utility>
#include <vector>

const char START_C = 'S';
const char SPLIT_C = '^';
const char EMPTY_C = '.';

int main() {
  std::println("Ciao, {}!", "Mondo");
  std::string line;
  auto is_splitter = [](auto c) { return c == SPLIT_C; };
  std::string full_input;
  // Parse input
  std::getline(std::cin, full_input);
  const auto manifold_width = std::size(full_input);
  while (std::getline(std::cin, line)) {
    full_input.append(std::move(line));
  }
  assert(full_input.size() % manifold_width == 0);
  const auto manifold_height = full_input.size() / manifold_width;
  const auto manifold =
      std::mdspan(full_input.data(), manifold_height, manifold_width);
  const auto start_it =
      std::find(full_input.cbegin(), full_input.cend(), START_C);
  const auto start_index =
      static_cast<std::size_t>(std::distance(full_input.cbegin(), start_it));
  // Part 1
  std::vector<bool> beams(manifold_width, false);
  auto new_beams = beams;
  beams[start_index] = true;
  int64_t beam_splits{0};
  // Part 2
  std::vector<int64_t> timelines(manifold_width, 0);
  auto new_timelines = timelines;
  timelines[start_index] = 1;

  // Solutions Part 1 and Part 2
  for (auto h = 1uz; h < manifold_height; ++h) {
    for (auto w = 0uz; w < manifold_width; ++w) {
      if (beams[w] && is_splitter(manifold[h, w])) {
        // Part 1
        new_beams[w] = false;
        new_beams[w - 1] = new_beams[w + 1] = true;
        ++beam_splits;
        // Part 2
        new_timelines[w] = 0;
        new_timelines[w - 1] += timelines[w];
        new_timelines[w + 1] += timelines[w];
      } else {
        new_beams[w] = beams[w] || new_beams[w];
        new_timelines[w] = timelines[w] + new_timelines[w];
      }
    }
    std::swap(beams, new_beams);
    std::ranges::fill(new_beams, false);
    std::swap(timelines, new_timelines);
    std::ranges::fill(new_timelines, 0);
  }

  std::println("Solution part 1: {}", beam_splits);
  std::println("Solution part 2: {}",
               std::ranges::fold_left(timelines, 0, std::plus()));
}
