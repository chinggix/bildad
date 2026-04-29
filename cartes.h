#ifndef CARTES_H
#define CARTES_H

#ifndef M_PI
#define M_PI 	3.1415926
#endif
#define EPS 	0.0001

#define MIN(a, b) 	((a) < (b) ? (a) : (b))
#define ZEROF(a) 	((a) > -EPS && (a) < EPS)
#define SQU(a) 		((a) * (a))
#define DET2(a, b) 	((a).x * (b).y - (a).y * (b).x)
#define SGN(a) 		(ZEROF(a) ? 0 : ((a) > EPS ? 1 : -1))
#define DIST(a, b) 	(hypot((a).x - (b).x, (a).y - (b).y))
#define ALONG(a, b, v) \
	((v) > EPS ? ((b) - (a) > EPS * (v)) : ((b) - (a) < EPS * (v)))
#define PTALONG(a, b, v) \
	(ALONG((a).x, (b).x, (v).x) && ALONG((a).y, (b).y, (v).y))

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
	struct vec2 ll; /* Lower-left */
	struct vec2 ur; /* Upper-right*/
};

double 	rect_width(struct rect);
double 	rect_length(struct rect);
int line_rect_cross(struct vec2, struct vec2, struct rect, struct vec2 *);
int near_line_circle_itx(struct vec2, struct vec2, struct vec2, double,
			 struct vec2 *);
int 	is_inside_rect(struct vec2, struct rect);

#endif /* CARTES_H */
