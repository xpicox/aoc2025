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
#include <numeric>
#include <print>
#include <span>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

// input reading taken from:
// https://marcoarena.wordpress.com/2016/03/13/cpp-competitive-programming-io/
struct custom_delims : std::ctype<char> {
  custom_delims() : ctype<char>(make_table()) {}

  static const mask *make_table() {
    static mask table[ctype<char>::table_size];
    table[' '] = table[','] = table['\n'] = ctype_base::space;
    return &table[0];
  }
};

using cord_t = int64_t;
using vec3_t = std::array<cord_t, 3>;

static int64_t distance(const vec3_t &first, const vec3_t &second) {
  int64_t inner = std::inner_product(
      first.begin(), first.end(), second.begin(), 0L, std::plus<>(),
      [](auto a, auto b) { return (a - b) * (a - b); });
  return inner;
}

class union_find {
public:
  using set_id_t = std::size_t;
  union_find(std::size_t n_sets) : _sets(n_sets, 0), _sizes(n_sets, 1) {
    std::iota(_sets.begin(), _sets.end(), 0);
  }

  std::size_t merge_sets(set_id_t set1, set_id_t set2) {
    auto root1 = find_set_id(set1);
    auto root2 = find_set_id(set2);
    if (root1 == root2) {
      return _sizes[root1];
    }
    auto [new_root, new_child] = std::minmax(root1, root2);
    _sets[new_child] = new_root;
    _sizes[new_root] += _sizes[new_child];
    return _sizes[new_root];
  }

  auto merge_sets(std::pair<set_id_t, set_id_t> sets) {
    auto [set1, set2] = sets;
    return merge_sets(set1, set2);
  }

  std::vector<std::size_t> get_sets_sizes() {
    std::vector<std::size_t> result;
    for (auto &set : _sets) {
      if (set == _sets[set]) { // root of a set
        result.push_back(_sizes[set]);
      }
    }
    return result;
  }

private:
  set_id_t find_set_id(set_id_t set) {
    if (_sets[set] != set) {
      _sets[set] = find_set_id(_sets[set]);
    }
    return _sets[set];
  }

private:
  std::vector<set_id_t> _sets;
  std::vector<std::size_t> _sizes;
};

template <class T, class Proj = std::identity> class min_heap {
public:
  using element_t = T;
  using iterator = typename std::vector<element_t>::iterator;
  explicit min_heap(const std::vector<T> &elements, Proj proj = {}) noexcept
      : _data(elements), _heap(_data), _proj(proj) {
    std::ranges::make_heap(_heap, _comp, _proj);
  }
  explicit min_heap(std::vector<T> &&elements, Proj proj = {}) noexcept
      : _data(std::move(elements)), _heap(_data), _proj(proj) {
    std::ranges::make_heap(_heap, _comp, _proj);
  }

  iterator pop_min() {
    iterator result = std::ranges::pop_heap(_heap, _comp, _proj);
    _heap = _heap.first(_heap.size() - 1);
    return result;
  }

  std::size_t size() { return _heap.size(); }

  bool empty() { return _heap.empty(); }

private:
  std::vector<element_t> _data;
  std::span<element_t> _heap;
  Proj _proj;
  std::ranges::greater _comp{};
};

// Class template argument deduction rules
template <class T, class Proj>
min_heap(const std::vector<T> &, Proj) -> min_heap<T, Proj>;
template <class T, class Proj>
min_heap(std::vector<T> &&, Proj) -> min_heap<T, Proj>;

struct connection_t {
  std::size_t jbox1;
  std::size_t jbox2;
  int64_t distance;
  operator std::pair<std::size_t, std::size_t>() const {
    return {jbox1, jbox2};
  }
};

int main() {
  std::println("Ciao, {}!", "Mondo");
  // Parse input
  std::cin.imbue(std::locale(std::cin.getloc(), new custom_delims()));
  std::vector<vec3_t> boxes;
  while (std::cin >> std::ws) {
    boxes.push_back({});
    for (cord_t &cord : boxes.back()) {
      std::cin >> cord >> std::ws;
    }
  }
  const auto n_boxes = std::size(boxes);
  // Each box is identified by its position in the vector `boxes`
  // connection_t stores the distance between two boxes
  std::vector<connection_t> connections;
  for (auto box1 = 0uz; box1 < n_boxes; ++box1) {
    for (auto box2 = box1 + 1; box2 < n_boxes; ++box2) {
      connections.emplace_back(box1, box2, distance(boxes[box1], boxes[box2]));
    }
  }
  auto heap = min_heap(connections, &connection_t::distance);
  // make each junction box a circuit on its own
  union_find circuits(n_boxes);

  // Solution Part 1
  for (auto i = 0; i < 1000; ++i) {
    circuits.merge_sets(*heap.pop_min());
  }
  auto circuits_sizes = circuits.get_sets_sizes();
  std::ranges::sort(circuits_sizes, std::ranges::greater{});
  auto result_1 =
      std::accumulate(circuits_sizes.cbegin(), circuits_sizes.cbegin() + 3, 1uz,
                      std::multiplies<std::size_t>());

  // Solution Part 2
  decltype(heap.pop_min()) last_connection_it;
  uint64_t last_set_size = 0;
  while (!heap.empty() && last_set_size != n_boxes) {
    last_connection_it = heap.pop_min();
    last_set_size = circuits.merge_sets(*last_connection_it);
  }

  assert(last_set_size == n_boxes);
  auto [box1, box2, _d] = *last_connection_it;
  auto result_2 = std::get<0>(boxes[box1]) * std::get<0>(boxes[box2]);

  std::println("Solution part 1: {}", result_1);
  std::println("Solution part 2: {}", result_2);
}
