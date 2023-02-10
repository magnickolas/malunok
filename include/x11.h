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
    unsigned int width;
    unsigned int height;
    unsigned int depth;
    Pixmap pixmap;
    GC gc;
    XImage* image;
  } winProps;
  Display* dis;

  void initWinProps(int screen);
  void setRootAtoms();
};
