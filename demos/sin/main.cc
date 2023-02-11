#include "map_coords.h"
#include "motion_params/loop.h"
#include "motion_params/metamove.h"
#include "motion_params/move.h"
#include "motion_params/timer.h"
#include "sdl.h"
#include "sdl_utils.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <chrono>
#include <cmath>
#include <memory>
#include <thread>
#include <utility>

using namespace std::chrono_literals;

#define COLOR(c) (c).r, (c).g, (c).b, (c).a

int main() {
  int width = 1280;
  int height = 720;

  SDLContext sdl_ctx{SDL_INIT_VIDEO};
  auto& window_ctx = sdl_ctx.new_window(SDLWinContext::opts{
      .window =
          {
              .title = "malunok: sin demo",
              .x = SDL_WINDOWPOS_CENTERED,
              .y = SDL_WINDOWPOS_CENTERED,
              .w = width,
              .h = height,
              .flags = SDL_WINDOW_SHOWN | SDL_WINDOW_BORDERLESS,
          },
      .renderer = {
          .index = -1,
          .flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC,
      }});
  auto& window = window_ctx.get_window();
  auto& renderer = window_ctx.get_renderer();

  auto phase_var = Move(
      0.f,
      [](float phase, int64_t milli) {
        return phase + static_cast<float>(milli) / 50.f;
      },
      std::make_shared<Timer>());

  auto amplitude_var =
      Loop(
          0.5f,
          [](float amplitude, int64_t milli) {
            return amplitude + static_cast<float>(milli) / 3000.0f;
          },
          std::make_shared<Timer>())
          .end_value(2.f);

  auto freq_var = Loop(
                      0.5f,
                      [](float freq, int64_t milli) {
                        return freq + static_cast<float>(milli) / 5000.0f;
                      },
                      std::make_shared<Timer>())
                      .end_value(1.5f);

  auto color_slider = Loop(
                          0.f,
                          [](float v, int64_t milli) {
                            return v + static_cast<float>(milli) / 2000.f;
                          },
                          std::make_shared<Timer>())
                          .end_value(1.f);

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
    puts("-1");
    SharedTimer::tick();
    puts("0");
    FPSCapGuard g{60};
    puts("1");

    while (auto e = sdl_ctx.poll()) {
      printf("%d\n", e->type);
      if (e->type == SDL_QUIT ||
          (e->type == SDL_KEYDOWN && e->key.keysym.sym == SDLK_ESCAPE)) {
        goto SDL_EXIT;
      }
      //handle resize
      if (e->type == SDL_WINDOWEVENT &&
          e->window.event == SDL_WINDOWEVENT_RESIZED) {
        width = e->window.data1;
        height = e->window.data2;
      }
    }
    puts("2");
    /* SDL_GetWindowSize(&window, &width, &height); */
    const CoordMapper map_coords{width, height,
                                 static_cast<float>(width) / 10.f,
                                 static_cast<float>(height) / 5.f};
    // fill background
    const auto& cur_bg_color = bg_color.get();
    SDL_SetRenderDrawColor(&renderer, COLOR(cur_bg_color));
    SDL_RenderClear(&renderer);
    // sin
    const auto& cur_line_color = line_color.get();
    SDL_SetRenderDrawColor(&renderer, cur_line_color.r, cur_line_color.g,
                           cur_line_color.b, cur_line_color.a);
    puts("3");
    auto x_from = map_coords.from_screen(0, 0).first;
    auto x_to = map_coords.from_screen(width, 0).first;
    // dynamic variables
    auto cur_amp = amplitude_var.get();
    auto cur_freq = freq_var.get();
    auto cur_phase = phase_var.get();
    puts("4");

    std::vector<SDL_Point> points;
    float dmin = 1e-3f;
    float dmax = 1 / (10 * cur_freq);
    float step;
    puts("5");
    for (float x = x_from; x < x_to; x += step) {
      float y = std::sin(2 * PI * cur_freq * x + cur_phase) * cur_amp;
      step = dmax - (dmax - dmin) / cur_amp * abs(y);
      auto [dx, dy] = map_coords.to_screen(x, y);
      points.push_back({dx, dy});
    }
    puts("6");
    SDL_RenderDrawLines(&renderer, points.data(),
                        static_cast<int>(points.size()));
    SDL_RenderPresent(&renderer);
    puts("7");
    puts("DONE");
  }
SDL_EXIT:
  return 0;
}
