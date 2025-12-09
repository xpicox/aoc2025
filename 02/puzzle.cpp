#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <locale>
#include <numeric>
#include <ostream>
#include <print>
#include <ranges>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

// input reading taken from:
// https://marcoarena.wordpress.com/2016/03/13/cpp-competitive-programming-io/
struct custom_delims : std::ctype<char> {
  custom_delims() : ctype<char>(make_table()) {}

  static const mask *make_table() {
    static mask table[ctype<char>::table_size];
    table[' '] = table[','] = table['-'] = ctype_base::space;
    return &table[0];
  }
};

using range_t = std::pair<std::string, std::string>;

std::string ten_to_the_power_of(std::size_t digits) {
  std::string v(digits + 1, '0');
  v[0] = '1';
  return v;
}

std::string nines(std::size_t digits) {
  std::string v(digits, '9');
  return v;
}

void collect_invalid_ids_1(range_t range, std::vector<int64_t> &invalid) {
  // NOTE: A repeated sequence appears only in even-sized strings
  // 1. Normalise left and right, to be of even size, increment up to right
  // bound
  auto& [fst, snd] = range;
  const auto fst_even = fst.size() % 2 == 0;
  const auto snd_even = snd.size() % 2 == 0;
  if (!fst_even) {
    // round_up(fst);
    fst = ten_to_the_power_of(fst.size());
    if (fst >= snd) { // TODO: e se di lunghezza diversa? che succede?
      return;
    }
  }
  if (!snd_even) {
    // round_down(snd);
    snd = nines(snd.size() - 1);
    if (snd <= fst) {
      return;
    }
  }
  const auto snd_n = std::stoul(snd);
  const auto fst_n = std::stoul(fst);
  auto current = fst;
  auto current_n = fst_n;
  while (current_n <= snd_n) {
    const auto mid = current.size() / 2;
    std::copy_n(current.begin(), mid, std::next(current.begin(), mid));
    current_n = std::stoul(current);
    if (current_n >= fst_n && current_n <= snd_n) {
      invalid.push_back(current_n);
      // std::cout << current << std::endl;
    }
    // increment current
    current_n += std::pow(10, mid);
    current = std::to_string(current_n);
    // TODO: check if multiple of 2
  }
}

void collect_invalid_ids_2(const range_t& range, const std::size_t repetitions,
                           std::vector<int64_t> &invalid_ids) {
  auto& [fst, snd] = range;
  const auto fst_n = std::stoul(fst);
  const auto snd_n = std::stoul(snd);
  auto current = fst;
  auto current_n = fst_n;
  do {
    const auto rem = current.size() % repetitions;
    if (rem != 0) {
      current = ten_to_the_power_of(current.size() + repetitions - rem - 1);
      assert(current.size() % repetitions == 0);
    }
    const auto window_size = current.size() / repetitions;
    const auto begin = current.begin();
    auto dest = begin;
    for (int j = 1; j < repetitions; ++j) {
      std::advance(dest, window_size);
      std::copy_n(begin, window_size, dest);
    }
    current_n = std::stoul(current);
    if (current_n >= fst_n && current_n <= snd_n) {
      invalid_ids.push_back(current_n);
    }
    current_n += std::pow(10, window_size * (repetitions - 1));
    current = std::to_string(current_n);
  } while (current_n <= snd_n);
}

int main() {
  // std::println("Ciao, {}!", "Mondo");
  std::string line;
  std::getline(std::cin, line);
  std::istringstream ss{std::move(line)};
  ss.imbue(std::locale(std::cin.getloc(), new custom_delims()));

  std::vector<std::string> v{};
  std::copy(std::istream_iterator<std::string>(ss),
            std::istream_iterator<std::string>(), std::back_inserter(v));

  std::vector<range_t> ranges;
  for (auto first = v.begin(); first != v.end(); ++first) {
    auto second = std::next(first);
    if (second == v.end())
      break;
    ranges.emplace_back(std::move(*first), std::move(*second));
    first = second;
  }
  // Part 1
  std::vector<int64_t> invalid_ids;
  for (auto &s : ranges) {
    collect_invalid_ids_1(s, invalid_ids);
    // check_invalid_rep(s, 2, invalid_ids);
  }
  std::cout << "Solution part 1: "
            << std::ranges::fold_left(invalid_ids, 0u, std::plus<>())
            << std::endl;
  invalid_ids.clear();
  // Part 2
  for (auto &s : ranges) {
    for (int rep = 2; rep <= s.second.size(); ++rep) {
      collect_invalid_ids_2(s, rep, invalid_ids);
    }
  }
  std::sort(invalid_ids.begin(), invalid_ids.end());
  auto last = std::unique(invalid_ids.begin(), invalid_ids.end());
  invalid_ids.erase(last, invalid_ids.end());
  std::cout << "Solution part 2: "
            << std::ranges::fold_left(invalid_ids, 0, std::plus<>())
            << std::endl;
}
