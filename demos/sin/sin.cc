#include "motion_params/loop.h"
#include "motion_params/metamove.h"
#include "motion_params/move.h"
#include "sdl.h"
#include "sdl_utils.h"
#include "x11.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <X11/Xlib.h>
#include <chrono>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <memory>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <thread>

using namespace std::chrono_literals;

#define COLOR(c) (c).r, (c).g, (c).b, (c).a

int main() {
  int width = 1280;
  int height = 720;

  SDLContext sdl_ctx{SDL_INIT_VIDEO};
  auto& window_ctx = sdl_ctx.new_window({
      .title = "malunok: sin demo",
      .x = SDL_WINDOWPOS_CENTERED,
      .y = SDL_WINDOWPOS_CENTERED,
      .w = width,
      .h = height,
      .flags = SDL_WINDOW_SHOWN | SDL_WINDOW_BORDERLESS,
      .renderer_index = -1,
      .renderer_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC,
  });
  auto& window = window_ctx.get_window();
  auto& renderer = window_ctx.get_renderer();

  auto phase = Move(0., [](const float& phase, const long& milli) {
    return phase + milli / 50.f;
  });

  auto amplitude = Loop(0.5f, [](const float& amplitude, const long& milli) {
                     return amplitude + milli / 3000.0f;
                   }).end_value(2.f);

  auto freq = Loop(0.5f, [](const float& freq, const long& milli) {
                return freq + milli / 5000.0f;
              }).end_value(1.5f);

  auto color_slider = Loop(0.f, [](const float& v, const long& milli) {
                        return v + milli / 2000.f;
                      }).end_value(1);
  auto bg_color = MetaMove(
      BLACK,
      [](const Color& color, const float& k) {
        return color + (GRAY - color) * k;
      },
      color_slider);
  auto line_color = MetaMove(
      LIGHT_BLUE,
      [](const Color& color, const float& k) {
        return color + (YELLOW - color) * k;
      },
      color_slider);

  while (true) {
    while (auto e = sdl_ctx.poll()) {
      if (e->type == SDL_QUIT ||
          (e->type == SDL_KEYDOWN && e->key.keysym.sym == SDLK_ESCAPE)) {
        goto SDL_EXIT;
      }
    }
    SDL_GetWindowSize(&window, &width, &height);
    const CoordMapper map_coords{width, height, [](int w, int h) {
                                   return std::make_pair(w / 20, h / 20);
                                 }};
    // fill background
    const auto& cur_bg_color = bg_color.get();
    SDL_SetRenderDrawColor(&renderer, COLOR(cur_bg_color));
    SDL_RenderClear(&renderer);
    // sin
    const auto& cur_line_color = line_color.get();
    SDL_SetRenderDrawColor(&renderer, cur_line_color.r, cur_line_color.g,
                           cur_line_color.b, cur_line_color.a);
    auto x_from = map_coords.from_screen(0, 0).first;
    auto x_to = map_coords.from_screen(width, 0).first;
    // dynamic variables
    auto cur_amp = amplitude.get();
    auto cur_freq = freq.get();
    auto cur_phase = phase.get();

    for (double x = x_from; x < x_to; x += 0.1) {
      double y = sin(2 * PI * cur_freq * x + cur_phase) * cur_amp;
      auto [dx, dy] = map_coords.to_screen_f(x, y);
      SDL_DrawDisk(&renderer, dx, dy, 5);
    }
    SDL_RenderPresent(&renderer);
  }
SDL_EXIT:
  return 0;
}
