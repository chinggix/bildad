#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "game.h"

enum game game_type = BLANK;
struct ball cue_ball;
struct ball obj_ball[MAX_OBJ];
unsigned int obj_num; /* Number of object balls */
double radius;
struct rect field; /* Playing field */
struct cue stick;

static void gamefatal(const char *, ...);
static void game_null_test(void);

void gamefatal(const char *msg, ...)
{
  va_list ap;

  fputs("GAME ERROR: ", stderr);

  va_start(ap, msg);
  vfprintf(stderr, msg, ap);
  va_end(ap);

  exit(1);
}

void game_null_test()
{
  if (game_type == BLANK)
    gamefatal("Initalize game first!\n");
}

struct vec2 aim_cue()
{
  int i;
  struct vec2 res;
  struct rect tqud; /* rectangle for collision testing */

  game_null_test();

  for (i = 0; i < obj_num; ++i) {
    if (near_line_circle_itx(cue_ball.pos, stick.drc, obj_ball[i].pos,
                             2 * radius, &res))
      return res;
  }

  tqud = field;
  tqud.ll.x += radius;
  tqud.ll.y += radius;
  tqud.ur.x -= radius;
  tqud.ur.y -= radius;

  line_rect_cross(cue_ball.pos, stick.drc, tqud, &res);

  return res;
}

void rotate_cue(double angle)
{
  double x, y;

  x = stick.drc.x;
  y = stick.drc.y;

  if (angle < 0) {
    angle = 2 * M_PI + angle;
  }

  stick.drc.x = x * cos(angle) - y * sin(angle);
  stick.drc.y = x * sin(angle) + y * cos(angle);
}
