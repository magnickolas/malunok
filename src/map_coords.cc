#include "map_coords.h"

namespace {
template <typename T> T clip(T value, T min, T max) {
  return (value < min) ? min : (value > max) ? max : value;
}
} // namespace

CoordMapper::CoordMapper(int width, int height, float grid_unit_x,
                         float grid_unit_y)
    : width_(width), height_(height), grid_unit_x_(grid_unit_x),
      grid_unit_y_(grid_unit_y) {}

std::pair<int, int> CoordMapper::to_screen(float x, float y) const {
  int dx = static_cast<int>(x * grid_unit_x_) + width_ / 2;
  int dy = static_cast<int>(y * grid_unit_y_) + height_ / 2;
  return {clip(dx, 0, width_ - 1), clip(dy, 0, height_ - 1)};
}
std::pair<float, float> CoordMapper::to_screen_f(float x, float y) const {
  float dx = x * grid_unit_x_ + static_cast<float>(width_) / 2.f;
  float dy = y * grid_unit_y_ + static_cast<float>(height_) / 2.f;
  return {clip(dx, 0.0f, static_cast<float>(width_ - 1)),
          clip(dy, 0.0f, static_cast<float>(height_ - 1))};
}
std::pair<float, float> CoordMapper::from_screen(int dx, int dy) const {
  float x = (static_cast<float>(dx) - static_cast<float>(width_) / 2.f) /
            grid_unit_x_;
  float y = (static_cast<float>(dy) - static_cast<float>(height_) / 2.f) /
            grid_unit_y_;
  return {x, y};
}
