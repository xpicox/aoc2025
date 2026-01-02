#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <map>
#include <mdspan>
#include <print>
#include <set>
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

using u64 = uint64_t;
using i64 = int64_t;
using cord_t = i64;
using vec2_t = std::array<cord_t, 2>; // coordinates x,y

// compute the area of the rectangle described by the two corners p and q
static i64 rectangle_area(vec2_t p, vec2_t q) {
  const auto base = 1 + std::abs(p[0] - q[0]);
  const auto height = 1 + std::abs(p[1] - q[1]);
  return base * height;
}

using dextents_t = std::dextents<i64, 2>;
template <typename T> using grid2D_t = std::mdspan<T, dextents_t>;

const char BG_COLOR = '.';
const char FG_COLOR = 'X';

class compressed_coordintes_mapper {
  std::vector<i64> compressed_to_x;
  std::vector<i64> compressed_to_y;
  std::map<i64, i64> x_to_compressed;
  std::map<i64, i64> y_to_compressed;

public:
  compressed_coordintes_mapper(const std::vector<vec2_t> &coordinates_set) {
    compressed_to_x.reserve(coordinates_set.size());
    compressed_to_y.reserve(coordinates_set.size());

    for (auto &cord : coordinates_set) {
      compressed_to_x.push_back(cord[0]);
      compressed_to_y.push_back(cord[1]);
    }
    std::ranges::sort(compressed_to_x);
    std::ranges::sort(compressed_to_y);

    auto range =
        std::ranges::unique(compressed_to_x.begin(), compressed_to_x.end());
    compressed_to_x.erase(range.begin(), range.end());
    range = std::ranges::unique(compressed_to_y.begin(), compressed_to_y.end());
    compressed_to_y.erase(range.begin(), range.end());

    i64 compressed_cord{};
    for (auto real_x : compressed_to_x) {
      x_to_compressed.emplace(real_x, compressed_cord++);
    }
    compressed_cord = 0;
    for (auto real_y : compressed_to_y) {
      y_to_compressed.emplace(real_y, compressed_cord++);
    }
  }

  vec2_t real_to_compressed(vec2_t point) {
    return {x_to_compressed[point[0]], y_to_compressed[point[1]]};
  }

  vec2_t compressed_to_real(vec2_t point) {
    return {compressed_to_x[static_cast<u64>(point[0])],
            compressed_to_y[static_cast<u64>(point[1])]};
  }

  i64 compressed_x_size() { return std::ssize(compressed_to_x); }
  i64 compressed_y_size() { return std::ssize(compressed_to_y); }
};

template <typename T> class canvas_t {
  grid2D_t<T> _canvas;
  T _bg_color;
  T _fill_color;

public:
  canvas_t(grid2D_t<T> grid, T bg_color, T fill_color)
      : _canvas(grid), _bg_color(bg_color), _fill_color(fill_color) {}

  void draw_polygon(const std::vector<vec2_t> &polygon) {
    draw_edges(polygon);
    fill_polygon(polygon);
  }
  bool is_rectangle_filled(vec2_t p, vec2_t q) {
    auto actual_area = rectangle_area(p, q);
    auto [row_min, row_max] = std::minmax(p[1], q[1]);
    auto [col_min, col_max] = std::minmax(p[0], q[0]);
    auto filled_area{0L};
    for (auto row = row_min; row <= row_max; ++row) {
      for (auto col = col_min; col <= col_max; ++col) {
        filled_area += _canvas[row, col] == _fill_color;
      }
    }
    return actual_area == filled_area;
  }

private:
  void draw_edges(const std::vector<vec2_t> &polygon) {
    for (auto i = 1uz; i < polygon.size(); ++i) {
      auto [from_x, from_y] = polygon[i - 1];
      auto [to_x, to_y] = polygon[i];
      auto [row_min, row_max] = std::minmax(from_y, to_y);
      auto [col_min, col_max] = std::minmax(from_x, to_x);
      for (auto row = row_min; row <= row_max; ++row) {
        for (auto col = col_min; col <= col_max; ++col) {
          _canvas[row, col] = _fill_color;
        }
      }
    }
  }

  void fill_polygon(const std::vector<vec2_t> &polygon) {
    enum class POSITIONING { EDGE, OUTSIDE, INSIDE };
    for (auto row = 0L; row < _canvas.extent(0); ++row) {
      auto positioning = POSITIONING::OUTSIDE;
      for (auto col = 0L; col < _canvas.extent(1); ++col) {
        auto &pixel = _canvas[row, col];
        switch (positioning) {
        case POSITIONING::OUTSIDE:
          if (pixel == _fill_color) {
            positioning = POSITIONING::INSIDE;
          }
          break;
        case POSITIONING::INSIDE:
          if (pixel == _fill_color) {
            positioning = POSITIONING::EDGE;
          } else {
            pixel = _fill_color;
          }
          break;
        case POSITIONING::EDGE:
          if (pixel == _bg_color) {
            if (point_in_polygon({col, row}, polygon)) {
              positioning = POSITIONING::INSIDE;
              pixel = _fill_color;
            } else {
              positioning = POSITIONING::OUTSIDE;
            }
          }
          break;
        }
      }
    }
  }

  // Geometric functions

  // from, to form an edge
  // function returns positive number if P is on the left side of the edge
  // function returns negative number if P is on the right side of the edge
  // function returns 0 if point is on the edge
  static i64 edge_function(vec2_t point, vec2_t from, vec2_t to) {
    return (point[0] - from[0]) * (to[1] - from[1]) -
           (point[1] - from[1]) * (to[0] - from[0]);
  }

  static bool is_on_edge(vec2_t point, vec2_t from, vec2_t to) {
    vec2_t direction{to[0] - from[0], to[1] - from[1]};
    if (direction[0] == 0) { // vertical edge
      auto between_y_cords = (point[1] - from[1]) * (point[1] - to[1]) <= 0;
      return point[0] == from[0] && between_y_cords;
    } else if (direction[1] == 0) { // horizontal edge
      auto between_x_cords = (point[0] - from[0]) * (point[0] - to[0]) <= 0;
      return point[1] == from[1] && between_x_cords;
    }
    assert(false);
  }

  // Test whether point lies inside the polygon, taken from:
  // https://web.archive.org/web/20130126163405/http://geomalgorithms.com/a03-_inclusion.html#wn_PnPoly()
  // and adapted to use edge_function instead of isLeft and made more robust by
  // first checking whether point lies on the edge. edge_function was taken from
  // https://www.scratchapixel.com/lessons/3d-basic-rendering/rasterization-practical-implementation/rasterization-stage.html
  static bool point_in_polygon(vec2_t point,
                               const std::vector<vec2_t> &polygon) {
    i64 winding_number = 0;
    for (auto i = 1uz; i < polygon.size(); ++i) {
      auto &start = polygon[i - 1];
      auto &end = polygon[i];
      auto edge = std::pair(i - 1, i);
      if (is_on_edge(point, start, end)) {
        return true;
      }
      if (start[1] <= point[1]) { // point is below start
        if (end[1] > point[1]) {  // point is above end
          if (edge_function(point, start, end) <
              0) { // point on the right of edge
            ++winding_number;
          }
        }
      } else {                    // point is above start
        if (end[1] <= point[1]) { // point is below end
          if (edge_function(point, start, end) >
              0) { // point on the left of edge
            --winding_number;
          }
        }
      }
    }
    return winding_number != 0;
  }
};

int main() {
  std::println("Ciao, {}!", "Mondo");
  // Parse input
  std::cin.imbue(std::locale(std::cin.getloc(), new custom_delims()));
  std::vector<vec2_t> red_tiles;
  while (std::cin >> std::ws) {
    red_tiles.push_back({});
    for (cord_t &cord : red_tiles.back()) {
      std::cin >> cord >> std::ws;
    }
  }

  // Solution Part 1
  auto result_1 = 0L;
  for (auto p = red_tiles.cbegin(); p != red_tiles.cend(); ++p) {
    for (auto q = std::next(p); q != red_tiles.cend(); ++q) {
      if (rectangle_area(*p, *q) > result_1) {
        result_1 = rectangle_area(*p, *q);
      }
    }
  }

  // Solution Part 2
  auto cc_map = compressed_coordintes_mapper(red_tiles);
  auto compress_vec2_t = [&cc_map](vec2_t point) -> vec2_t {
    return cc_map.real_to_compressed(point);
  };
  auto real_vec2_t = [&cc_map](vec2_t point) -> vec2_t {
    return cc_map.compressed_to_real(point);
  };

  std::vector<vec2_t> polygon;
  polygon.reserve(red_tiles.size() + 1);
  std::ranges::transform(red_tiles, std::back_inserter(polygon),
                         compress_vec2_t);
  polygon.push_back(polygon[0]);
  const auto width = cc_map.compressed_x_size() + 1;
  const auto height = cc_map.compressed_y_size();

  std::string draw_buffer(static_cast<u64>(width * height), BG_COLOR);

  auto grid = grid2D_t<char>(draw_buffer.data(), height, width);
  for (auto row = 0L; row < grid.extent(0); ++row) {
    grid[row, width - 1] = '\n';
  }

  // Paint the polygon on a canvas and check whether rectangles described by red
  // tiles are fully filled with color
  canvas_t<char> canvas{grid, BG_COLOR, FG_COLOR};
  canvas.draw_polygon(polygon);
  std::println("{}", draw_buffer);

  auto result_2{0L};
  for (auto p = polygon.cbegin(); p != polygon.cend(); ++p) {
    for (auto q = std::next(p); q != polygon.cend(); ++q) {
      if (canvas.is_rectangle_filled(*p, *q)) {
        auto real_p = real_vec2_t(*p);
        auto real_q = real_vec2_t(*q);
        auto area = rectangle_area(real_p, real_q);
        if (area > result_2) {
          result_2 = area;
        }
      }
    }
  }

  std::println("Solution part 1: {}", result_1);
  std::println("Solution part 2: {}", result_2);
}
