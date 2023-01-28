#pragma once

#include <X11/Xlib.h>
#include <cstdint>
#include <memory>

struct X11Background {
public:
  X11Background();
  ~X11Background();
  void setPixmap(std::unique_ptr<uint8_t[]>& pixels);

public:
  struct WinProps {
    Window root;
    int screen;
    int width;
    int height;
    int depth;
    Pixmap pixmap;
    GC gc;
    XImage* image;
  } winProps;
  Display* dis;

  WinProps getWinProps(int screen);
  void setRootAtoms();
};
