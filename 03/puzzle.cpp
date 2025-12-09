#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <print>
#include <string>
#include <vector>

int main() {
  std::println("Ciao, {}!", "Mondo");
  std::string line;
  int64_t result_1{0};
  int64_t result_2{0};

  while (std::getline(std::cin, line)) {
    // PART 1
    auto decimal = line.cbegin();
    auto unit = std::next(decimal);
    for (auto fst = decimal, snd = unit; fst != std::prev(line.end());
         ++fst, ++snd) {
      if (*fst > *decimal) {
        decimal = fst;
        unit = snd;
      } else if (*snd > *unit) {
        unit = snd;
      }
    }
    result_1 += std::stoi(std::string{*decimal, *unit});

    // PART 2
    const std::size_t n_batteries{12};
    std::string s_batteries(n_batteries, '0');
    std::vector<std::string::const_iterator> v_batteries(
        n_batteries); // vector of iterators to positions in line
    auto beg = line.cbegin();
    std::generate_n(v_batteries.begin(), n_batteries,
                    [&beg]() { return beg++; });
    auto batteries_left = line.size();
    for (auto current = line.cbegin(); current != line.cend();
         ++current, --batteries_left) {
      auto assignable_batteries = std::min(n_batteries, batteries_left);
      auto batteries_begin = std::prev(v_batteries.end(), assignable_batteries);
      auto battery_it = std::find_if(
          batteries_begin, v_batteries.end(),
          [&current](const auto &battery_it) {
            return current > battery_it && // current is past stored iterator
                   *current > *battery_it;
          });
      auto new_it = current;
      std::generate(battery_it, v_batteries.end(),
                    [&new_it]() { return new_it++; });
    }
    std::transform(v_batteries.cbegin(), v_batteries.cend(),
                   s_batteries.begin(), [](auto &it) { return *it; });
    result_2 += std::stoll(s_batteries);
  }

  std::println("Solution part 1: {}", result_1);
  std::println("Solution part 2: {}", result_2);
}
