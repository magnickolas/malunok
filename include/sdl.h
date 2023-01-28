#pragma once

#include "x11.h"
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
public:
  struct win_opts {
    const char* title;
    int x;
    int y;
    int w;
    int h;
    uint32_t flags;
  };
  struct renderer_opts {
    int index;
    uint32_t flags;
  };
  struct opts {
    win_opts window;
    renderer_opts renderer;
  };

public:
  SDLWinContext(SDLWinContext&&);
  SDLWinContext(opts o);
  SDLWinContext(const X11Background& x11, const renderer_opts& renderer_opts);
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

  template <typename... Args> SDLWinContext& new_window(Args&&... args) {
    windows.emplace_back(std::forward<Args>(args)...);
    return windows.back();
  }
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
