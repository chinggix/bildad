#include <math.h>
#include "cartes.h"

static struct vec2 translate_xy(struct vec2, double, double);
static int is_alongside(struct vec2, struct vec2, struct vec2);

int
is_alongside(struct vec2 tail, struct vec2 head, struct vec2 drc)
{
  double x;

  x = head.x - tail.x;

  if (SGN(drc.x) * SGN(x) > 0) return 1;
  return 0;
}

struct vec2
translate_xy(struct vec2 point, double x, double y)
{
  struct vec2 res;
  res.x = point.x + x;
  res.y = point.y + y;

  return res;
}

double
rect_width(struct rect quad)
{
  return fabs(quad.ur.x - quad.ll.x);
}

double
rect_length(struct rect quad)
{
  return fabs(quad.ur.y - quad.ll.y);
}

/* Particle point (prt) and its direction (drc)
 * calculate its nearest intersection with a given
 * circle (org, r). Notice that the sign of the
 * direction is important.
 * return values => 0 is not intersect
 * 		    1 is otherwise
 */
int
near_line_circle_cross(struct vec2 prt, struct vec2 drc,
		       struct vec2 org, double r, struct vec2 *res)
{
  double del, len, prl, x0, y0;
  struct vec2 arw, crs1, crs2;

  /* For simplicity, translate the cordinates origin to the circle origin
   * Will revert back later when returning value.
   */
  prt = translate_xy(prt, -org.x, -org.y);

  /* Arrow of vector from given point */
  arw.x = prt.x + drc.x;
  arw.y = prt.x + drc.y;

  prl = DET2(prt, arw);
  len = hypot(drc.x, drc.y);
  del = SQU(r * len) - SQU(prl);

  if (del < 0) return 0;

  x0 =   prl * drc.y;
  y0 = - prl * drc.x;

  /* First case: tangent */
  if (del == 0) {
    crs1.x = x0;
    crs1.y = y0;
    if (!is_alongside(prt, crs1, drc))
      return 0;
  }

  /* Second case: intersection */
  if (del > 0) {
    /* First intersected point */
    crs1.x = (x0 + SGN(drc.y) * drc.x * sqrt(del)) / SQU(len);
    crs1.y = (y0 + fabs(drc.y) * sqrt(del)) / SQU(len);

    /* Second intersected point */
    crs2.x = (x0 - SGN(drc.y) * drc.x * sqrt(del)) / SQU(len);
    crs2.y = (y0 - fabs(drc.y) * sqrt(del)) / SQU(len);     

    if (!is_alongside(prt, crs1, drc) && !is_alongside(prt, crs2, drc))
      return 0;

    if (is_alongside(prt, crs1, drc) && is_alongside(prt, crs2, drc)) {
      if (DIST(prt, crs1) < DIST(prt, crs2))
	crs1 = crs2;
    } else if (!is_alongside(prt, crs1, drc)) {
      crs1 = crs2;
    }
  }

  crs1 = translate_xy(crs1, org.x, org.y);
  res->x = crs1.x;
  res->y = crs1.y;

  return 1;
}

int
line_rect_cross(struct vec2 prt, struct vec2 drc,
		struct rect quad, struct vec2 *res)
{
  struct vec2 tpt;
  double rax, ray;

  if (!is_alongside(prt, quad.ll, drc))
    tpt = quad.ur;
  else
    tpt = quad.ll;

  rax = (drc.x == 0 ? 1000000.0 : (tpt.x - prt.x) / drc.x);
  ray = (drc.y == 0 ? 1000000.0 : (tpt.y - prt.y) / drc.y);

  if (ray < rax) {
    res->x = prt.x + rax * drc.x;
    res->y = tpt.y;
  } else {
    res->x = tpt.x;
    res->y = prt.y + ray * drc.y;
  }

  /* TODO: handle the case testing point is outside rectangle */
  return 1;
}
