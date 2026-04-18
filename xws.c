#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <GL/glx.h>

#include "draw.h"
#include "xws.h"

XEvent event;

struct {
  Display *dpy;
  Window win;
  int scr;
  XVisualInfo *vis;
  GLXContext ctx;
} window;

static int attr[] = {
  GLX_RGBA,
  GLX_RED_SIZE,   1,
  GLX_GREEN_SIZE, 1,
  GLX_BLUE_SIZE,  1,
  GLX_DEPTH_SIZE, 1,
  None  
};

static void 	 xfatal(const char*, ...);
static Window	 dumpwin(void);

Window
dumpwin()
{
  /*
   * Dumb dimensions, since window managers would change these anyway
   */
  int bor = 0, top = 0, left = 0;
  unsigned int w = 680, h = 760;

  Window root = XRootWindow(window.dpy, window.scr);
  Colormap cmap = XCreateColormap(window.dpy, root,
				  window.vis->visual,
				  AllocNone);
  XSetWindowAttributes swa;
  swa.colormap = cmap;
  swa.border_pixel = bor;
  swa.event_mask = ExposureMask | ButtonPressMask | StructureNotifyMask;

  return XCreateWindow(window.dpy, root, top, left, w, h, bor,
		       window.vis->depth, InputOutput, window.vis->visual,
		       CWBorderPixel | CWColormap | CWEventMask, &swa);
}

void
xfatal(const char *msg, ...)
{
  va_list ap;

  fputs("X ERROR: ", stderr);

  va_start(ap, msg);
    vfprintf(stderr, msg, ap);
  va_end(ap);

  exit(1);
}

void
run_x()
{
  
  for (;;) {
    redraw();
  }
}

void
setup_xevent()
{
  XWindowAttributes xwa;
  
  while (XNextEvent(window.dpy, &event)) {
      if (event.type == MapNotify) break;
  }

  XGetWindowAttributes(window.dpy, window.win, &xwa);
  reshape_viewport(xwa.width, xwa.height);
}

void 
initialize_x()
{
  if (!(window.dpy = XOpenDisplay(NULL))) 
    xfatal("Can not open display!\n");

  window.scr = XDefaultScreen(window.dpy);

  if (!(window.vis = glXChooseVisual(window.dpy, window.scr, attr)))
    xfatal("Failed to get Visual\n");

  window.ctx = glXCreateContext(window.dpy, window.vis, None, 1);
  window.win = dumpwin();

  glXMakeCurrent(window.dpy, window.win, window.ctx);
  XMapWindow(window.dpy, window.win);
}
