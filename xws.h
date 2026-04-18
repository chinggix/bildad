#ifndef XWF_H
#define XWF_H

#include <X11/Xlib.h>
#include <GL/glx.h>

extern XEvent event;

void setup_xevent(void);
void initialize_x(void);
void run_x(void);

#endif /* XWF_H */
