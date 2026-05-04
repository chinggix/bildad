#include <math.h>
#include <stdlib.h>
#include "cartes.h"

static struct vec2 translate_xy(struct vec2, double, double);

struct vec2 translate_xy(struct vec2 point, double x, double y)
{
	point.x += x;
	point.y += y;
	return point;
}

double rect_width(struct rect quad)
{
	return fabs(quad.ur.x - quad.ll.x);
}

double rect_length(struct rect quad)
{
	return fabs(quad.ur.y - quad.ll.y);
}

int near_line_circle_itx(struct vec2 pnt, struct vec2 drc, struct vec2 org,
			 double r, struct vec2 *res)
{
	double A, B, C;
	double dis, dvr, mul, x0, y0;
	/* Return values */
	struct vec2 a, b;

	pnt = translate_xy(pnt, -org.x, -org.y);

	/* linear equation Ax + By + C = 0 */
	A = -drc.y;
	B = drc.x;
	C = DET2(pnt, drc);

	dvr = SQU(A) + SQU(B);

	if (SQU(C) > SQU(r) * dvr + EPS)
		return 0;

	x0 = -A * C / dvr;
	y0 = -B * C / dvr;

	if (ZEROF(SQU(C) - SQU(r) * dvr)) {
		res->x = x0;
		res->y = y0;

		if (!PTALONG(pnt, *res, drc))
			return 0;

		*res = translate_xy(*res, org.x, org.y);
		return 1;
	}

	dis = SQU(r) - SQU(C) / dvr;
	mul = sqrt(dis / dvr);

	a.x = x0 + B * mul;
	a.y = y0 - A * mul;
	b.x = x0 - B * mul;
	b.y = y0 + A * mul;

	if (PTALONG(pnt, a, drc) && PTALONG(pnt, b, drc)) {
		if (hypot(pnt.x - a.x, pnt.y - a.y) <
		    hypot(pnt.x - b.x, pnt.y - b.y)) {
			res->x = a.x;
			res->y = a.y;
		} else {
			res->x = b.x;
			res->y = b.y;
		}
	} else if (PTALONG(pnt, a, drc)) {
		res->x = a.x;
		res->y = a.y;
	} else if (PTALONG(pnt, b, drc)) {
		res->x = b.x;
		res->y = b.y;
	} else {
		return 0;
	}

	*res = translate_xy(*res, org.x, org.y);
	return 1;
}

/*
 * TODO: This function assume that vector always find its cut.
 * 	 It would be a good ideas to handle uncut cases also.
 */
int line_rect_cross(struct vec2 pnt, struct vec2 drc, struct rect rec,
		    struct vec2 *res)
{
	struct vec2 fst, snd;
	double x0, y0;
	double dx, dy;

	fst = translate_xy(rec.ll, -pnt.x, -pnt.y);
	snd = translate_xy(rec.ur, -pnt.x, -pnt.y);

	if (ZEROF(drc.x) || ZEROF(drc.y)) {
		if (ZEROF(drc.x)) {
			if (drc.y > 0) {
				x0 = 0;
				y0 = snd.y;
			} else {
				x0 = 0;
				y0 = fst.y;
			}
		} else if (ZEROF(drc.y)) {
			if (drc.x > 0) {
				x0 = snd.x;
				y0 = 0;
			} else {
				x0 = fst.x;
				y0 = 0;
			}
		}
		res->x = x0;
		res->y = y0;
		*res = translate_xy(*res, pnt.x, pnt.y);
		return 1;
	}

	if (drc.x > 0 && drc.y > 0) {
		x0 = snd.x;
		y0 = snd.y;
	} else if (drc.x > 0 && drc.y < 0) {
		x0 = snd.x;
		y0 = fst.y;
	} else if (drc.x < 0 && drc.y < 0) {
		x0 = fst.x;
		y0 = fst.y;
	} else {
		x0 = fst.x;
		y0 = snd.y;
	}

	/*
	 * drc.x * gradient = x0
	 * drc.y * gradient = y0
	 */

	/* Test with y = y0 */
	dx = x0 / drc.x;
	dy = y0 / drc.y;

	if (dx < dy) {
		res->x = x0;
		res->y = dx * drc.y;
	} else {
		res->x = dy * drc.x;
		res->y = y0;
	}

	*res = translate_xy(*res, pnt.x, pnt.y);
	return 1;
}

int
is_inside_rect(struct vec2 pnt, struct rect qud)
{
	if (pnt.x < qud.ll.x || pnt.y < qud.ll.y)
		return 0;

	if (pnt.x > qud.ur.x || pnt.y > qud.ur.y)
		return 0;

	return 1;
}

/*
 * Twirl the original coordiate (x, y) to a new one (a, b) by angle.
 */
struct vec2 
twirl_coor2(struct vec2 pnt, double ang)
{
	struct vec2 res;
	
	res.x = pnt.x * cos(ang) - pnt.y * sin(ang);
	res.y = pnt.x * sin(ang) + pnt.y * cos(ang);

	return res;
}

struct rect
zoom_rect(struct rect quad, double factor)
{
	struct rect res;

	res.ur.x = quad.ur.x + factor;
	res.ur.y = quad.ur.y + factor;
	res.ll.x = quad.ll.x - factor;
	res.ll.y = quad.ll.y - factor;

	return res;
}

/*
 * Spare rectangle variables into an array
 */
double*
sparse_rect(struct rect quad)
{
	double *res;

	res = (double *) calloc(4, sizeof(double));

	if (res != NULL) {
		res[0] = quad.ll.x;
		res[1] = quad.ll.y;
		res[2] = quad.ur.x;
		res[3] = quad.ur.y;
	}

	return res;
}

/*
 * Sparse all edges of rectangle
 */
double*
sparse_rect_all_edges(struct rect quad)
{
	double *ps, *rs;

	ps = sparse_rect(quad);
	if (!ps)
		return NULL;

	rs = (double *) calloc(8, sizeof(double));	
	if (rs != NULL) {
		rs[0] = ps[2];
		rs[1] = ps[0];
		rs[2] = ps[0];
		rs[3] = ps[2];

		rs[4] = ps[3];
		rs[5] = ps[3];
		rs[6] = ps[1];
		rs[7] = ps[1];
	}

	free(ps);
	return rs;
}
