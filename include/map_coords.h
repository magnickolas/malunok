#pragma once

#include <utility>

class CoordMapper {
public:
  CoordMapper(int width, int height, float grid_unit_x, float grid_unit_y);
  std::pair<int, int> to_screen(float x, float y) const;
  std::pair<float, float> to_screen_f(float x, float y) const;
  std::pair<float, float> from_screen(int dx, int dy) const;

private:
  int width_;
  int height_;
  float grid_unit_x_;
  float grid_unit_y_;
};

template <typename T> T clip(T value, T min, T max) {
  return (value < min) ? min : (value > max) ? max : value;
}

inline std::pair<int, int> to_screen(float x, float y, int width, int height,
                                     int grid_unit_x, int grid_unit_y) {
  int dx = static_cast<int>(x * grid_unit_x) + width / 2;
  int dy = static_cast<int>(y * grid_unit_y) + height / 2;
  return {clip(dx, 0, width - 1), clip(dy, 0, height - 1)};
}
inline std::pair<float, float> to_screen_f(float x, float y, int width,
                                           int height, int grid_unit_x,
                                           int grid_unit_y) {
  float dx = x * grid_unit_x + width / 2.f;
  float dy = y * grid_unit_y + height / 2.f;
  return {clip(dx, 0.0f, static_cast<float>(width - 1)),
          clip(dy, 0.0f, static_cast<float>(height - 1))};
}
inline std::pair<float, float> from_screen(int dx, int dy, int width,
                                           int height, int grid_unit_x,
                                           int grid_unit_y) {
  float x = (dx - width / 2.f) / grid_unit_x;
  float y = (dy - height / 2.f) / grid_unit_y;
  return {x, y};
}
