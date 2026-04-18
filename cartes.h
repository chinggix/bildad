#ifndef CARTES_H
#define CARTES_H

#define SQU(a) 		((a) * (a))
#define DET2(a, b)	((a).x * (b).y - (a).y * (b).x)
#define SGN(a)		((a) < 0 ? -1 : 1)
#define DIST(a, b) 	(hypot((a).x - (b).x, (a).y - (b).y))

struct vec2 {
  double x;
  double y;
};

struct vec3 {
  double x;
  double y;
  double z;
};

/*
 * Sides parallel with coordinates
 */
struct rect {
  struct vec2 ll;	/* Lower-left */
  struct vec2 ur;	/* Upper-right*/
};

double 	 rect_width(struct rect);
double 	 rect_length(struct rect);
int	 near_line_circle_cross(struct vec2,
				struct vec2,
				struct vec2,
				double,
				struct vec2*);
int	 line_rect_cross(struct vec2, struct vec2, struct rect, struct vec2*);


#endif	/* CARTES_H */
