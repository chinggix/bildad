#ifndef XWF_H
#define XWF_H

#include <GL/glx.h>
#include <X11/Xlib.h>

extern XEvent event;

void setup_xevent(void);
void initialize_x(void);
void run_x(void);

#endif /* XWF_H */
