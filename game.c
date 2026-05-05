#include <string.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "game.h"

static int 	coll_test[MAX_OBJ];

enum game 	game_type = BLANK;
struct ball 	cue_ball;
struct ball 	obj_ball[MAX_OBJ];
unsigned int 	obj_num; /* Number of object balls */
double 		radius;
double 		mass;
struct 		rect field; /* Playing field */
struct 		cue stick;

static void		gamefatal(const char *, ...);
static void		game_null_test(void);
static int		is_halt(void);
static void 		descent_vlo(struct vec2*);
static void 		descent_rot(struct vec3*);
static int 		is_vlo_vanish(struct ball);
static int 		is_rot_vanish(struct ball);
static int 		is_stop_sliding(struct ball);
static enum collision	test_collision(struct ball);
static struct ball 	wander(struct ball, double); 
static struct ball 	rotate(struct ball, double);
static struct ball 	slide(struct ball, double);
static struct ball 	rail_impact(struct ball, double);
static void		ball_impact(struct ball*, int, double);
static struct rect 	wander_area(void);
static struct ball 	railly_forward_slide(struct ball, double);
static struct ball 	railly_slide_stick(struct ball, double);
static double 		railly_sign(struct ball);
static void 		bally_clash(struct ball*, struct ball*, double);

void 
gamefatal(const char *msg, ...)
{
	va_list ap;

	fputs("GAME ERROR: ", stderr);

	va_start(ap, msg);
	vfprintf(stderr, msg, ap);
	va_end(ap);

	exit(1);
}

void 
game_null_test()
{
	if (game_type == BLANK)
		gamefatal("Initalize game first!\n");
}

int
is_vlo_vanish(struct ball b)
{
	return ISSTOP(b.vlo.x) && ISSTOP(b.vlo.y);
}

int
is_rot_vanish(struct ball b)
{
	return ISSTOP(b.rot.x) && ISSTOP(b.rot.y);
}

int 
is_halt()
{
	int i;

	if (!is_vlo_vanish(cue_ball))
		return 0;
	if (!is_rot_vanish(cue_ball))
		return 0;

	for (i = 0; i < obj_num; ++i) {
		if (!is_vlo_vanish(obj_ball[i]))
			return 0;
		if (!is_rot_vanish(obj_ball[i]))
			return 0;
	}

	return 1;
}

void
descent_vlo(struct vec2 *vlo)
{
	double mag;

	mag = hypot(vlo->x, vlo->y);

	if (abs(mag) < STOP) {
		vlo->x = 0.0;
		vlo->y = 0.0;
	} else if (abs(mag) < 0.5) {
		vlo->x /= 2.0;
		vlo->y /= 2.0;
	} else if (abs(mag) < 20.0) {
		vlo->x -= ETA * vlo->x;
		vlo->y -= ETA * vlo->y;
	}
}

void
descent_rot(struct vec3 *rot)
{
	double mag;

	mag = hypot(rot->z, hypot(rot->x, rot->y));
	printf("MAG: %f\n", mag);

	if (abs(mag) < STOP) {
		rot->x = 0.0;
		rot->y = 0.0;
		rot->z = 0.0;
	} else if (abs(mag) < 0.2) {
		rot->x /= 2.0;
		rot->y /= 2.0;
		rot->z /= 2.0;
	} else if (abs(mag) < 5.0) {
		rot->x -= ETA * rot->x / 10;
		rot->y -= ETA * rot->y / 10;
		rot->z -= ETA * rot->z / 10;
	} else if (abs(mag) < 20.0) {
		rot->x -= ETA * rot->x / 30;
		rot->y -= ETA * rot->y / 30;
		rot->z -= ETA * rot->z / 30;
	}
}

struct ball
slide(struct ball b, double dt)
{
	double dvr, vax, vay;
	b.pos.x += dt * b.vlo.x;
	b.pos.y += dt * b.vlo.y;

	/* Verlocities at contact point */
	vax = b.vlo.x - radius * b.rot.y;
	vay = b.vlo.y + radius * b.rot.x;

	dvr = hypot(vax, vay);
	b.vlo.x -= dt * MI * G0 * vax / dvr;
	b.vlo.y -= dt * MI * G0 * vay / dvr;

	b.rot.x -= dt * (5 / 2) * MI * G0 * vay / (dvr * radius);
	b.rot.y += dt * (5 / 2) * MI * G0 * vax / (dvr * radius);
	b.rot.z += dt * (5 / 2) * MUZ * SGN(b.rot.z) / (mass * SQU(radius));

	return b;
}

struct ball
rotate(struct ball b, double dt)
{
	double dvr;
	double vx, vy;
	
	dvr = hypot(b.rot.x, b.rot.y);

	/*
	b.pos.x += dt * b.vlo.x;
	b.pos.y += dt * b.vlo.y;

	b.vlo.x -= dt * 5 * MUY * b.rot.y / (mass * radius * dvr * 7);
	b.vlo.y += dt * 5 * MUX * b.rot.x / (mass * radius * dvr * 7);
	*/

	vx = 5 * MUY * b.rot.y / (mass * radius * dvr * 7);
        vy = 5 * MUX * b.rot.x / (mass * radius * dvr * 7);

	b.pos.x += dt * vx;
	b.pos.y += dt * vy;

	b.rot.x -= dt * 5 * MUX * G0 * b.rot.x / 
	             	(mass * dvr * SQU(radius) * 7);
	b.rot.y -= dt * 5 * MUY * G0 * b.rot.y / 
	             	(mass * dvr * SQU(radius) * 7);
	b.rot.z -= dt * 5 * MUZ * G0 * b.rot.z / 
			(mass * dvr * SQU(radius) * 7);

	printf("ROTATE\n");
	printf("%f\t%f\t%f\n", b.rot.x, b.rot.y, b.rot.z);
	return b;
}

double
railly_sign(struct ball b)
{
	struct rect quad;

	quad = wander_area();

	if (b.pos.x <= quad.ll.x || b.pos.x >= quad.ur.x)
		return -1.0;

	return 1.0;
}

struct ball
railly_forward_slide(struct ball b, double dt)
{
	double sign;

	sign = railly_sign(b);

	b.vlo.x -= dt * b.vlo.x *
		   (1 - (1 + VAPR) * cos(THETA) * 
		   (MI * cos(THETA) * sin(THETA) + cos(THETA)));
	b.vlo.y -= dt * (b.vlo.y + MI * (1 + VAPR) * cos(THETA)
				      * sin(THETA) * b.vlo.x);
	
	b.vlo.x *= sign;
	b.vlo.y *= -sign;

	return b;
}

struct ball
railly_slide_stick(struct ball b, double dt)
{
	double sign;

	sign = railly_sign(b);

	b.vlo.x += dt * (
		   b.vlo.x * 
		   (1 - 2/7 * SQU(sin(THETA)) - 
		    (1 + VAPR) * SQU(cos(THETA))) -
		   2/7 * radius * b.rot.y * sin(THETA));
	b.vlo.y += dt * (5/7 * b.vlo.y + 
		   2/7 * radius * (b.rot.x * sin(THETA) - 
				   b.rot.z * cos(THETA)));
	
	b.vlo.x *= sign;
	b.vlo.y *= -sign;

	return b;
}

struct ball
rail_impact(struct ball b, double dt)
{
	if (b.sliding) {
		b = railly_forward_slide(b, dt);
	} else {
		b = railly_slide_stick(b, dt);
	}

	return b;
}

void
bally_clash(struct ball *ba, struct ball *bb, double dt)
{
	double ang, far, near;
	struct vec2 spra, sprb, resa, resb;

	far  = bb->pos.y - ba->pos.y;
	near = bb->pos.x - ba->pos.x;
	
	if (ZEROF(far))
		ang = near < EPS ? M_PI : 0.0;
	else if (ZEROF(near))
		ang = far < EPS ? 3 * M_PI / 2 : M_PI / 2;
	else 
		ang = atan(far / near);

	printf("IMPACT ANGLE:\t%f\n", ang * (180.0 / M_PI));
	spra = twirl_coor2(ba->vlo, -ang);
	sprb = twirl_coor2(bb->vlo, -ang);
	
	resa.x = spra.x * (1 - VAPR) / 2 + sprb.x * (1 + VAPR) / 2;
	resb.x = spra.x * (1 + VAPR) / 2 + sprb.x * (1 - VAPR) / 2;
	resa.y = spra.y;
	resb.y = sprb.y;
	
	resa = twirl_coor2(resa, ang);
	resb = twirl_coor2(resb, ang);

	ba->vlo = resa;
	bb->vlo = resb;
}

void
ball_impact(struct ball *b, int idx, double dt)
{
	int i;
	double d, v;
	struct ball *btest;

	if (coll_test[idx])
		return;

	for (btest = &cue_ball, i = 0; i <= obj_num; ++i) {
		d = DIST(b->pos, btest->pos);
		v = (hypot(b->vlo.x, b->vlo.y) + 
		     hypot(btest->vlo.x, btest->vlo.y)) / 2;

		if (!ZEROF(d) && (d <= 2*radius + v + STOP) && !coll_test[i]) {
			bally_clash(b, btest, dt);

			b->sliding = 1;
			btest->sliding = 1;

			*b = wander(*b, dt);
			*btest = wander(*btest, dt);

			coll_test[idx] = 1;
			coll_test[i] = 1;

			return;
		}

		if (i != obj_num)
			btest = obj_ball + i;
	}
}

int
is_stop_sliding(struct ball b)
{
	return (abs(b.vlo.x - radius * b.rot.y) < STOP &&
		abs(b.vlo.y + radius * b.rot.x) < STOP) ||
		is_vlo_vanish(b);
}

struct ball
wander(struct ball b, double dt)
{
	struct ball res;

	res = b;

	if (b.sliding && !is_stop_sliding(b)) {
		res = slide(res, dt);
		descent_vlo(&res.vlo);
		return res;
	}

	res.sliding = 0;

	if (!is_rot_vanish(b)) {
		res = rotate(res, dt);
		/* descent_vlo(&res.vlo); */
		descent_rot(&res.rot);
	} else {
		halt_motion(&res);
	}

	return res;
}

enum collision
test_collision(struct ball b)
{
	int i;
	double d, v;
	struct ball btest;
	struct rect area;

	area = wander_area();
	v = hypot(b.vlo.x, b.vlo.y);
	area = zoom_rect(area, -v * STOP);

	if (!is_inside_rect(b.pos, area))
		return RAIL;

	for (btest = cue_ball, i = 0; i <= obj_num; ++i) {
		d = DIST(b.pos, btest.pos);
		if (!ZEROF(d) && (d < 2 * radius))
			return BALL;
		if (i != obj_num)
			btest = obj_ball[i];
	}

	return BED;
}

struct vec2 
aim_cue()
{
	int i, is_ballcl;
	struct vec2 res, dum;
	struct rect tqud; /* rectangle for collision testing */

	game_null_test();

	is_ballcl = 0;

	for (i = 0; i < obj_num; ++i) {

		if (!near_line_circle_itx(cue_ball.pos, stick.drc,
					  obj_ball[i].pos, 2 * radius, &dum))
			continue;

		if (!is_ballcl) {
			res = dum;
			is_ballcl = 1;
		} else if (DIST(cue_ball.pos, dum) <
			   DIST(cue_ball.pos, res)) {
			res = dum;
		}
	}

	if (is_ballcl)
		return res;

	tqud = wander_area();

	line_rect_cross(cue_ball.pos, stick.drc, tqud, &res);

	return res;
}

struct rect
wander_area()
{
	struct rect res;
	res.ll.x = field.ll.x + radius;
	res.ll.y = field.ll.y + radius;
	res.ur.x = field.ur.x -	radius;
	res.ur.y = field.ur.y - radius;
	
	return res;
}

void 
adjust_cue(double angle)
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
void 
strike_cue_ball(double acc, double ang, double r)
{
	/* Omega */
	double omg;

	cue_ball.vlo.x = acc * stick.drc.x;
	cue_ball.vlo.y = acc * stick.drc.y;

	cue_ball.rot.z = (r / radius) * sin(ang);
	omg = (r / radius) * cos(ang);

	cue_ball.rot.x = omg * stick.drc.x;
	cue_ball.rot.y = omg * stick.drc.y;

	cue_ball.sliding = 1;
}

int
motion()
{
	int i;
	double dt;
	enum collision coll;
	struct ball *btest;

	if (is_halt()) return 0;

	memset(coll_test, 0, (obj_num + 1) * sizeof(int));

	for (btest = &cue_ball, i = 0, dt = 0.01; i <= obj_num; ++i) {
		coll = test_collision(*btest);
		if (coll == RAIL) {
			*btest = rail_impact(*btest, dt);
			*btest = wander(*btest, dt);
		} else if (coll == BALL) {
			ball_impact(btest, i, dt);
		} else {
			*btest = wander(*btest, dt);
		}

		if (i != obj_num) btest = obj_ball + i;
	}

	return 1;
}
