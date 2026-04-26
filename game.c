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
double mass;
struct rect field; /* Playing field */
struct cue stick;

static void	gamefatal(const char *, ...);
static void	game_null_test(void);
static int	is_halt(void);
static void  	wander(struct ball*); 
static void 	rotate(struct ball*);
static void 	slide(struct ball*);
static int 	is_vlo_vanish(struct vec2*);

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

int is_halt()
{
	int i;

	if (!(ZEROF(cue_ball.vct.x) && ZEROF(cue_ball.vct.y)))
		return 0;
	if (!(ZEROF(cue_ball.vlo.x) && ZEROF(cue_ball.vlo.y)))
		return 0;
	if (!(ZEROF(cue_ball.rot.x) && ZEROF(cue_ball.rot.y) &&
	      ZEROF(cue_ball.rot.z)))
		return 0;

	for (i = 0; i < obj_num; ++i) {
		if (!(ZEROF(obj_ball[i].vlo.x) && ZEROF(obj_ball[i].vlo.y)))
			return 0;
		if (!(ZEROF(obj_ball[i].rot.x) && ZEROF(obj_ball[i].rot.y) &&
		      ZEROF(obj_ball[i].rot.z)))
			return 0;
	}

	return 1;
}

int 
is_vlo_vanish(struct vec2 *vlo)
{
	int qry;

	qry = 0;

	if (vlo->x < EPS) {
		qry = 1;
		vlo->x = 0.0;
	}
	if (vlo->y < EPS) {
		qry = 1;
		vlo->y = 0.0;
	}

	return qry;
}

void 
slide(struct ball *b)
{
	double dvr;
	(b->pos).x += (b->vlo).x;
	(b->pos).y += (b->vlo).y;
	
	dvr = hypot((b->vct).x, (b->vct).y);
	(b->vlo).x += MI * G0 * (b->vct).x / dvr;
	(b->vlo).y += MI * G0 * (b->vct).y / dvr;

	(b->rot).x += (5 / 2) * MI * G0 * (b->vct).y / (dvr * radius * -1);
	(b->rot).y += (5 / 2) * MI * G0 * (b->vct).x / (dvr * radius);
	(b->rot).z += (5 / 2) * MUZ * SGN((b->rot).z) / (mass * SQU(radius));

	(b->vct).x -= MUX;
	(b->vct).y -= MUY;
}

void 
rotate(struct ball *b)
{
	double dvr;
	(b->pos).x += (b->vlo).x;
	(b->pos).y += (b->vlo).y;
	
	dvr = hypot((b->rot).x, (b->rot).y);
	(b->vlo).x += 5 * MUY * (b->rot).y / (mass * radius * dvr * -7);
	(b->vlo).y += 5 * MUX * (b->rot).x / (mass * radius * dvr * -7);

	(b->rot).x += 5 * MUX * G0 * (b->rot).x / 
		      (mass * dvr * SQU(radius) * -7);
	(b->rot).y += 5 * MUY * G0 * (b->rot).y / 
		      (mass * dvr * SQU(radius) * -7);
	(b->rot).z += 5 * MUZ * G0 * (b->rot).z / 
		      (mass * dvr * SQU(radius) * -7);
}

void 
wander(struct ball *b)
{
	if (!is_vlo_vanish(&(b->vct))) {
		slide(b);
	} else if (!is_vlo_vanish(&(b->vlo))) {
		rotate(b);
	}
}

struct vec2 aim_cue()
{
	int i, is_ballcl;
	struct vec2 res, dum;
	struct rect tqud; /* rectangle for collision testing */

	game_null_test();

	is_ballcl = 0;

	for (i = 0; i < obj_num; ++i) {
		if (near_line_circle_itx(cue_ball.pos, stick.drc,
					 obj_ball[i].pos, 2 * radius, &dum)) {
			if (!is_ballcl) {
				res = dum;
				is_ballcl = 1;
			} else {
				if (DIST(cue_ball.pos, dum) <
				    DIST(cue_ball.pos, res))
					res = dum;
			}
		}
	}

	if (is_ballcl)
		return res;

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

/*
 * Initialize motion for the cue ball, the ang and r modeling contact point of
 * the cue with the cue ball. acc is just an acceleration factor.
 */
void strike_cue_ball(double acc, double ang, double r)
{
	/* Omega */
	double omg;

	cue_ball.vct.x = acc * stick.drc.x;
	cue_ball.vct.y = acc * stick.drc.y;

	cue_ball.rot.z = (r / radius) * sin(ang);
	omg = (r / radius) * cos(ang);

	cue_ball.rot.x = omg * stick.drc.x;
	cue_ball.rot.y = omg * stick.drc.y;
}

int
motion()
{
	if (!is_halt()) {
		wander(&cue_ball);
		return 1;
	} else {
		return 0;
	}		
}
