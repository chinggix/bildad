#include <math.h>
#include "cartes.h"

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
