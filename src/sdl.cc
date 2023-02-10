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
} // namespace

SDLWinContext::SDLWinContext(SDLWinContext&&) = default;

SDLWinContext::SDLWinContext(SDLWinContext::opts o)
    : window_(nullptr, &SDL_DestroyWindow),
      renderer_(nullptr, &SDL_DestroyRenderer) {
  sdl_must(SDL_Init(SDL_INIT_VIDEO));
  window_.reset(
      sdl_must(SDL_CreateWindow(o.window.title, o.window.x, o.window.y,
                                o.window.w, o.window.h, o.window.flags)));
  renderer_.reset(sdl_must(
      SDL_CreateRenderer(window_.get(), o.renderer.index, o.renderer.flags)));
}

SDLWinContext::SDLWinContext(const X11Background& x11, const renderer_opts& ro)
    : window_(nullptr, &SDL_DestroyWindow),
      renderer_(nullptr, &SDL_DestroyRenderer) {
  sdl_must(SDL_Init(SDL_INIT_VIDEO));
  window_.reset(sdl_must(
      SDL_CreateWindowFrom(reinterpret_cast<const void*>(x11.winProps.root))));
  renderer_.reset(
      sdl_must(SDL_CreateRenderer(window_.get(), ro.index, ro.flags)));
}

SDL_Window& SDLWinContext::get_window() { return *window_.get(); }

SDL_Renderer& SDLWinContext::get_renderer() { return *renderer_.get(); }

SDLContext::SDLContext(uint32_t flags) { sdl_must(SDL_Init(flags)); }

SDLContext::~SDLContext() { SDL_Quit(); }

std::optional<SDL_Event> SDLContext::poll() {
  if (!SDL_PollEvent(&event_)) {
    return std::nullopt;
  }
  return event_;
}
