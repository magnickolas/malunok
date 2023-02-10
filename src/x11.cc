#include "x11.h"
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <cstdint>
#include <memory>

X11Background::X11Background() {
  dis = XOpenDisplay(nullptr);
  initWinProps(DefaultScreen(dis));
}

void X11Background::initWinProps(int screen) {
  winProps.root = RootWindow(dis, screen);
  winProps.screen = screen;
  winProps.width = static_cast<unsigned int>(DisplayWidth(dis, screen));
  winProps.height = static_cast<unsigned int>(DisplayHeight(dis, screen));
  winProps.depth = static_cast<unsigned int>(DefaultDepth(dis, screen));
  winProps.pixmap = XCreatePixmap(dis, winProps.root, winProps.width,
                                  winProps.height, winProps.depth);
  winProps.gc = DefaultGC(dis, screen);
}

void X11Background::setRootAtoms() {
  Atom atom_root, atom_eroot, type;
  unsigned char *data_root, *data_eroot;
  int format;
  unsigned long length, after;

  atom_root = XInternAtom(dis, "_XROOTMAP_ID", true);
  atom_eroot = XInternAtom(dis, "ESETROOT_PMAP_ID", true);

  if (atom_root != None && atom_eroot != None) {
    XGetWindowProperty(dis, winProps.root, atom_root, 0L, 1L, false,
                       AnyPropertyType, &type, &format, &length, &after,
                       &data_root);

    if (type == XA_PIXMAP) {
      XGetWindowProperty(dis, winProps.root, atom_eroot, 0L, 1L, false,
                         AnyPropertyType, &type, &format, &length, &after,
                         &data_eroot);

      if (data_root && data_eroot && type == XA_PIXMAP &&
          *reinterpret_cast<Pixmap*>(data_root) ==
              *reinterpret_cast<Pixmap*>(data_eroot))
        XKillClient(dis, *reinterpret_cast<Pixmap*>(data_root));
    }
  }

  atom_root = XInternAtom(dis, "_XROOTPMAP_ID", false);
  atom_eroot = XInternAtom(dis, "ESETROOT_PMAP_ID", false);

  XChangeProperty(dis, winProps.root, atom_root, XA_PIXMAP, 32, PropModeReplace,
                  reinterpret_cast<const unsigned char*>(&winProps.pixmap), 1);
  XChangeProperty(dis, winProps.root, atom_eroot, XA_PIXMAP, 32,
                  PropModeReplace,
                  reinterpret_cast<const unsigned char*>(&winProps.pixmap), 1);
}

void X11Background::setPixmap(std::unique_ptr<uint8_t[]>& pixels) {
  setRootAtoms();
  auto image = XCreateImage(dis, CopyFromParent, winProps.depth, ZPixmap, 0,
                            reinterpret_cast<char*>(pixels.get()),
                            winProps.width, winProps.height, 32, 0);
  XKillClient(dis, AllTemporary);
  XSetCloseDownMode(dis, RetainTemporary);
  XSetWindowBackgroundPixmap(dis, winProps.root, winProps.pixmap);
  XPutImage(dis, winProps.pixmap, DefaultGC(dis, DefaultScreen(dis)), image, 0,
            0, 0, 0, winProps.width, winProps.height);
  XClearWindow(dis, winProps.root);
  XFlush(dis);
  XSync(dis, false);
  XFree(image);
}

X11Background::~X11Background() { XCloseDisplay(dis); }
