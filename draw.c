#include <GL/gl.h>

#include "draw.h"

static struct {
  double x;
  double y;
  unsigned int w;
  unsigned int h;
} video = {0.0f, 0.0f, 680.0f, 480.0f};

void
clear_screen()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void
reshape_viewport(unsigned int w, unsigned int h)
{
  glViewport(video.x, video.y, w, h);
  video.w = w;
  video.h = h;
}

void
initialize_gl()
{
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  glOrtho(video.x, video.w, video.y, video.h, -1.0f, 1.0f);
}
