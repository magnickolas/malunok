#pragma once

#include <SDL.h>
#include <SDL_events.h>
#include <SDL_render.h>
#include <SDL_video.h>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <vector>

class SDLContext;

class SDLWinContext {
private:
  struct opts {
    const char* title;
    int x;
    int y;
    int w;
    int h;
    uint32_t flags;
    int renderer_index;
    uint32_t renderer_flags;
  };

public:
  SDLWinContext(SDLWinContext&&);
  SDLWinContext(opts&& o);
  SDLWinContext(const SDLWinContext&) = delete;
  SDLWinContext& operator=(const SDLWinContext&) = delete;
  ~SDLWinContext() = default;

  SDL_Window& get_window() { return *window_.get(); }
  SDL_Renderer& get_renderer();

private:
  std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)> window_;
  std::unique_ptr<SDL_Renderer, decltype(&SDL_DestroyRenderer)> renderer_;

  friend class SDLContext;
};

class SDLContext {
public:
  SDLContext(uint32_t flags);
  SDLContext(SDLContext&) = delete;
  SDLContext& operator=(const SDLContext&) = delete;
  ~SDLContext();

  SDLWinContext& new_window(SDLWinContext::opts&&);
  std::optional<SDL_Event> poll();

private:
  std::vector<SDLWinContext> windows;
  SDL_Event event_;
};

class CoordMapper {
public:
  CoordMapper(int width, int height,
              std::function<std::pair<int, int>(int width, int height)>&&
                  get_grid_units);
  CoordMapper(int width, int height, int grid_unit_x, int grid_unit_y);

  std::pair<int, int> to_screen(float x, float y) const;
  std::pair<float, float> to_screen_f(float x, float y) const;
  std::pair<float, float> from_screen(int dx, int dy) const;

private:
  int width_;
  int height_;
  std::function<std::pair<int, int>(int, int)> get_grid_units_;
};
