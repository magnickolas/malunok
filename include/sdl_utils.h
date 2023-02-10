#pragma once

#include <SDL_render.h>
#include <cstdint>

constexpr float PI = 3.14159265358979323846f;

void SDL_DrawCircle(SDL_Renderer* renderer, int32_t cx, int32_t cy,
                    int32_t radius);

void SDL_DrawDisk(SDL_Renderer* renderer, int32_t cx, int32_t cy,
                  int32_t radius);

struct Color {
  uint8_t r;
  uint8_t g;
  uint8_t b;
  uint8_t a;

  Color operator+(const Color& other) const;
  Color operator-(const Color& other) const;
  Color operator*(float k) const;
};

constexpr Color BLACK{0x00, 0x00, 0x00, 0xFF};
constexpr Color GRAY{0x80, 0x80, 0x80, 0xFF};
constexpr Color YELLOW{0xFF, 0xFF, 0x00, 0xFF};
constexpr Color LIGHT_BLUE{0x00, 0xFF, 0xFF, 0xFF};
