#ifndef CARTES_H
#define CARTES_H

struct vec2 {
  double x;
  double y;
};

struct vec3 {
  double x;
  double y;
  double z;
};

struct rect {
  struct vec2 ll;	/* Lower-left */
  struct vec2 ur;	/* Upper-right*/
};

double rect_width(struct rect);
double rect_length(struct rect);

#endif	/* CARTES_H */
