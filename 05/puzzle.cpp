#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <mdspan>
#include <print>
#include <span>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

using food_id_t = int64_t;

class segment_tree {
public:
  using element_t = food_id_t;
  using range_t = std::pair<element_t, element_t>;
  using tree_t = std::vector<range_t>;
  using tree_it = tree_t::iterator;
  using const_tree_it = tree_t::const_iterator;
  using ssize_t = decltype(std::ssize(tree_t{}));
  ssize_t n_leafs;
  tree_t tree;

  segment_tree(const std::vector<range_t> &ranges)
      : n_leafs(std::ssize(ranges)) {
    assert(n_leafs > 0);
    tree = tree_t(2 * ranges.size() - 1);
    build_tree_h(ranges, std::begin(tree));
  }
  bool is_present(element_t element) const {
    return is_present_helper_it(std::cbegin(tree), element, n_leafs);
  }

private:
  range_t build_tree_h(std::span<const range_t> ranges, tree_it tree_it) {
    const auto size = std::ssize(ranges);
    const auto half_size = size / 2;
    if (ranges.size() == 1) {
      *tree_it = ranges[0];
      return *tree_it;
    }
    const auto left_child = tree_it + 1;
    const auto right_child = tree_it + 2 * half_size;
    const auto &[ll, lr] = build_tree_h(
        ranges.first(static_cast<std::size_t>(half_size)), left_child);
    const auto &[rl, rr] = build_tree_h(
        ranges.last(static_cast<std::size_t>(size - half_size)), right_child);
    *tree_it = {std::min(ll, lr), std::max(lr, rr)};
    return *tree_it;
  }

  static bool is_in_range(element_t element, range_t range) {
    return range.first <= element && element <= range.second;
  }

  // recursive
  bool is_present_helper(const_tree_it node, element_t element,
                         ssize_t leafs) const {
    const auto half_leafs = leafs / 2;
    auto left_child = node + 1;
    auto right_child = node + 2 * half_leafs;
    if (leafs == 1) {
      return is_in_range(element, *node);
    }
    if (is_in_range(element, *left_child)) {
      return is_present_helper(left_child, element, half_leafs);
    } else if (is_in_range(element, *right_child)) {
      return is_present_helper(right_child, element, leafs - half_leafs);
    }
    return false;
  }
  // iterative
  bool is_present_helper_it(const_tree_it node, element_t element,
                            ssize_t leafs) const {
    auto is_in_range = [element](range_t r) {
      return r.first <= element && element <= r.second;
    };
    if (leafs == 1) {
      return is_in_range(*node);
    }
    while (leafs > 1) {
      auto half_leafs = leafs / 2;
      auto left_child = node + 1;
      auto right_child = node + 2 * half_leafs;
      if (is_in_range(*left_child)) {
        node = left_child;
        leafs = half_leafs;
      } else if (is_in_range(*right_child)) {
        node = right_child;
        leafs -= half_leafs;
      } else {
        return false;
      }
    }
    return true;
  }
};

using range_t = segment_tree::range_t;

int main() {
  std::println("Ciao, {}!", "Mondo");
  // Parse ranges:
  std::string line;
  std::vector<range_t> ranges;
  std::vector<food_id_t> queries;
  while (std::getline(std::cin, line) && !line.empty()) {
    std::istringstream sline{std::move(line)};
    food_id_t l, r;
    char dash;
    sline >> l >> dash >> r;
    ranges.emplace_back(l, r);
  }
  // Parse queries
  std::copy(std::istream_iterator<food_id_t>(std::cin),
            std::istream_iterator<food_id_t>(), std::back_inserter(queries));

  std::ranges::sort(ranges);
  // Remove redundant ranges
  auto write_it = ranges.begin();
  std::for_each(std::next(write_it), ranges.end(), [&write_it](auto range) {
    if (write_it->second < range.first) {
      ++write_it;
      *write_it = range;
    } else if (write_it->second < range.second) {
      write_it->second = range.second;
    }
  });
  ranges.erase(std::next(write_it), ranges.end());
  // Part 1
  segment_tree st(ranges);
  int64_t fresh_ingredients{0};
  for (auto q : queries) {
    bool present = st.is_present(q);
    fresh_ingredients += present;
  }
  // Part 2
  int64_t total_fresh_ingredients =
      std::ranges::fold_left(ranges, 0z, [](food_id_t acc, range_t r) {
        auto values = r.second - r.first + 1;
        return acc + values;
      });

  std::println("Solution part 1: {}", fresh_ingredients);
  std::println("Solution part 2: {}", total_fresh_ingredients);
}
