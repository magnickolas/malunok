#include "sdl.h"
#include <SDL_error.h>
#include <iostream>
#include <stdexcept>

namespace {
void throw_sdl_error() { throw std::runtime_error(SDL_GetError()); }

void sdl_must(int code) {
  if (code < 0) {
    throw_sdl_error();
  }
}

template <class T> void sdl_must(T) = delete;

template <typename T> T* sdl_must(T* ptr = nullptr) {
  if (ptr == nullptr) {
    throw_sdl_error();
  }
  return ptr;
}

template <typename T> T clip(T value, T min, T max) {
  if (value < min) {
    return min;
  }
  if (value > max) {
    return max;
  }
  return value;
}
} // namespace

SDLWinContext::SDLWinContext(SDLWinContext&&) = default;

SDLWinContext::SDLWinContext(SDLWinContext::opts&& o)
    : window_(nullptr, SDL_DestroyWindow),
      renderer_(nullptr, SDL_DestroyRenderer) {
  sdl_must(SDL_Init(SDL_INIT_VIDEO));
  window_.reset(
      sdl_must(SDL_CreateWindow(o.title, o.x, o.y, o.w, o.h, o.flags)));
  renderer_.reset(sdl_must(
      SDL_CreateRenderer(window_.get(), o.renderer_index, o.renderer_flags)));
}

SDL_Renderer& SDLWinContext::get_renderer() {
  return *sdl_must(renderer_.get());
}

SDLContext::SDLContext(uint32_t flags) { sdl_must(SDL_Init(flags)); }

SDLContext::~SDLContext() { SDL_Quit(); }

SDLWinContext& SDLContext::new_window(SDLWinContext::opts&& o) {
  windows.emplace_back(std::move(o));
  return windows.back();
}

std::optional<SDL_Event> SDLContext::poll() {
  if (!SDL_PollEvent(&event_)) {
    return std::nullopt;
  }
  return event_;
}

CoordMapper::CoordMapper(
    int width, int height,
    std::function<std::pair<int, int>(int width, int height)>&& get_grid_units)
    : width_(width), height_(height),
      get_grid_units_(std::move(get_grid_units)) {}

CoordMapper::CoordMapper(int width, int height, int grid_unit_x,
                         int grid_unit_y)
    : width_(width), height_(height),
      get_grid_units_([grid_unit_x, grid_unit_y](int, int) {
        return std::make_pair(grid_unit_x, grid_unit_y);
      }) {}

std::pair<int, int> CoordMapper::to_screen(float x, float y) const {
  auto [grid_unit_x, grid_unit_y] = get_grid_units_(width_, height_);
  int dx = static_cast<int>(x * grid_unit_x) + width_ / 2;
  int dy = static_cast<int>(y * grid_unit_y) + height_ / 2;
  return {clip(dx, 0, width_ - 1), clip(dy, 0, height_ - 1)};
}

std::pair<float, float> CoordMapper::to_screen_f(float x, float y) const {
  auto [grid_unit_x, grid_unit_y] = get_grid_units_(width_, height_);
  float dx = x * grid_unit_x + width_ / 2.f;
  float dy = y * grid_unit_y + height_ / 2.f;
  return {clip(dx, 0.0f, static_cast<float>(width_ - 1)),
          clip(dy, 0.0f, static_cast<float>(height_ - 1))};
}

std::pair<float, float> CoordMapper::from_screen(int x, int y) const {
  auto [grid_unit_x, grid_unit_y] = get_grid_units_(width_, height_);
  float dx = (x - width_ / 2.f) / grid_unit_x;
  float dy = (y - height_ / 2.f) / grid_unit_y;
  return {dx, dy};
}
