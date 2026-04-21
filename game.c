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
static void is_halt(void);

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

	cue_ball.vlo.x = acc * stick.drc.x;
	cue_ball.vlo.y = acc * stick.drc.y;

	cue_ball.rot.z = (r / radius) * sin(ang);
	omg = (r / radius) * cos(ang);

	cue_ball.rot.x = omg * stick.drc.x;
	cue_ball.rot.y = omg * stick.drc.y;
}

int is_halt()
{
	int i;

	if (!(ZEROF(cue_ball.vlo.x) && ZEROF(cue_ball.vlo.y)))
		return 1;
	if (!(ZEROF(cue_ball.rot.x) && ZEROF(cue_ball.rot.y) &&
	      ZEROF(cue_ball.rot.z)))
		return 1;

	for (i = 0; i < obj_num; ++i) {
		if (!(ZEROF(obj_ball[i].vlo.x) && ZEROF(obj_ball[i].vlo.y)))
			return 1;
		if (!(ZEROF(obj_ball[i].rot.x) && ZEROF(obj_ball[i].rot.y) &&
		      ZEROF(obj_ball[i].rot.z)))
			return 1;
	}

	return 0;
}

int motioning()
{
	if (is_halt())
		return 0;
}
