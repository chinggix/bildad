#ifndef XWF_H
#define XWF_H

#include <X11/Xlib.h>
#include <GL/glx.h>

struct xws {
  Display *dpy;
  Window win;
  int scr;
  XVisualInfo *vis;
  GLXContext ctx;
};

extern XEvent event;

void setup_xevent(void (*)(unsigned int, unsigned int));
void initialize_x(void);

#endif /* XWF_H */
