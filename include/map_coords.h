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
