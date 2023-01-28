#include "sdl_utils.h"
#include <SDL_render.h>
#include <cstdint>

void SDL_DrawCircle(SDL_Renderer* renderer, int32_t cx, int32_t cy,
                    int32_t radius) {
  const int32_t diameter = (radius * 2);

  int32_t x = (radius - 1);
  int32_t y = 0;
  int32_t tx = 1;
  int32_t ty = 1;
  int32_t error = (tx - diameter);

  while (x >= y) {
    SDL_RenderDrawPoint(renderer, cx + x, cy - y);
    SDL_RenderDrawPoint(renderer, cx + x, cy + y);
    SDL_RenderDrawPoint(renderer, cx - x, cy - y);
    SDL_RenderDrawPoint(renderer, cx - x, cy + y);
    SDL_RenderDrawPoint(renderer, cx + y, cy - x);
    SDL_RenderDrawPoint(renderer, cx + y, cy + x);
    SDL_RenderDrawPoint(renderer, cx - y, cy - x);
    SDL_RenderDrawPoint(renderer, cx - y, cy + x);

    if (error <= 0) {
      ++y;
      error += ty;
      ty += 2;
    }

    if (error > 0) {
      --x;
      tx += 2;
      error += (tx - diameter);
    }
  }
}

void SDL_DrawDisk(SDL_Renderer* renderer, int32_t cx, int32_t cy,
                  int32_t radius) {
  const int32_t diameter = (radius * 2);

  int32_t x = (radius - 1);
  int32_t y = 0;
  int32_t tx = 1;
  int32_t ty = 1;
  int32_t error = (tx - diameter);

  while (x >= y) {
    SDL_RenderDrawLine(renderer, cx + x, cy - y, cx + x, cy + y);
    SDL_RenderDrawLine(renderer, cx - x, cy - y, cx - x, cy + y);
    SDL_RenderDrawLine(renderer, cx + y, cy - x, cx + y, cy + x);
    SDL_RenderDrawLine(renderer, cx - y, cy - x, cx - y, cy + x);

    if (error <= 0) {
      ++y;
      error += ty;
      ty += 2;
    }

    if (error > 0) {
      --x;
      tx += 2;
      error += (tx - diameter);
    }
  }
}

Color Color::operator+(const Color& other) const {
  return {r + other.r, g + other.g, b + other.b, a + other.a};
}

Color Color::operator-(const Color& other) const {
  return {r - other.r, g - other.g, b - other.b, a - other.a};
}

Color Color::operator*(double k) const {
  return {int(r * k), int(g * k), int(b * k), int(a * k)};
}
