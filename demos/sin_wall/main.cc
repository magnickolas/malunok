#include "map_coords.h"
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
#include <chrono>
#include <thread>
#include <utility>

using namespace std::chrono_literals;

#define COLOR(c) (c).r, (c).g, (c).b, (c).a

int main() {
  int width = 1920;
  int height = 1080;

  SDLContext sdl_ctx{SDL_INIT_VIDEO};
  X11Background background;
  auto& window_ctx = sdl_ctx.new_window(
      background,
      SDLWinContext::renderer_opts{.index = -1,
                                   .flags = SDL_RENDERER_ACCELERATED |
                                            SDL_RENDERER_PRESENTVSYNC});
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

  auto pixels =
      std::make_unique<uint8_t[]>(static_cast<size_t>(width * height * 4));
  while (true) {
    SharedTimer::tick();

    auto start = SDL_GetPerformanceCounter();

    while (auto e = sdl_ctx.poll()) {
      if (e->type == SDL_QUIT ||
          (e->type == SDL_KEYDOWN && e->key.keysym.sym == SDLK_ESCAPE)) {
        goto SDL_EXIT;
      }
    }
    SDL_GetWindowSize(&window, &width, &height);
    const CoordMapper map_coords{width, height,
                                 static_cast<float>(width) / 20.f,
                                 static_cast<float>(height) / 20.f};
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
    auto cur_amp = amplitude_var.get();
    auto cur_freq = freq_var.get();
    auto cur_phase = phase_var.get();

    for (float x = x_from; x < x_to; x += 0.1f) {
      float y = sin(2 * PI * cur_freq * x + cur_phase) * cur_amp;
      auto [dx, dy] = map_coords.to_screen(x, y);
      SDL_DrawDisk(&renderer, dx, dy, 5);
    }
    SDL_RenderReadPixels(&renderer, nullptr, SDL_PIXELFORMAT_BGRA32,
                         pixels.get(),
                         width * static_cast<int>(sizeof(uint32_t)));
    background.setPixmap(pixels);

    // Cap to 60 FPS
    auto end = SDL_GetPerformanceCounter();
    float elapsedMS = static_cast<float>(end - start) /
                      static_cast<float>(SDL_GetPerformanceFrequency()) *
                      1000.0f;
    SDL_Delay(std::max(static_cast<uint32_t>(16.666f - elapsedMS), 0u));
  }
SDL_EXIT:
  return 0;
}
