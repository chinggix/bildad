#include <GL/gl.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "draw.h"
#include "cartes.h"
#include "game.h"

/*
 * TODO: Something wrong with this dimensions
 */
static struct {
	double x;
	double y;
	double w;
	double h;
} video = { 0.0f, 0.0f, 760.0f, 680.0f };

static struct {
	struct vec2 org;
	double r;
	int drawing;
} inspect;

static double shadow = 0.3;

struct vec2 hit;
int is_motion;

static void drawfatal(const char *msg, ...);
static void initialize_gl(void);
static void cls(void);
static void draw_table(struct rect, double, double);
static void set_color(enum color);
static void draw_sphere(double);
static void draw_solid_sphere(double);
static void draw_cue_aim(enum color);
static void draw_inspect(void);
static void draw_curtain(void);
static void draw_inspecting_ball(void);
static void draw_inspecting_point(void);
static void draw_rounded_rect(struct rect, double);
static void cushion_shadow(struct rect);

void 
drawfatal(const char *msg, ...)
{
	va_list ap;

	fputs("DRAW ERROR: ", stderr);

	va_start(ap, msg);
	vfprintf(stderr, msg, ap);
	va_end(ap);

	exit(1);
}

void
draw_rounded_rect(struct rect quad, double r)
{
	double cenx, ceny, alpha, rang, *rp;
	int i, j, step;
	struct rect zoom;

	zoom = zoom_rect(quad, -r);
	rp = sparse_rect_all_edges(zoom);

	if (!rp)
		drawfatal("Unable to sparse rectangle!\n");

	cenx = quad.ll.x + quad.ur.x / 2;
	ceny = quad.ll.y + quad.ur.y / 2;

	step = 20;
	rang = 2 * M_PI / (step * 4);

	glBegin(GL_TRIANGLE_FAN);

	for (i = 0; i < 4; ++i) {
		for (j = 0; j <= step; ++j) {
			alpha = rang * (i * step + j);
			glVertex2f(rp[i] + r * cos(alpha), 
				   rp[4 + i] + r * sin(alpha));
		}
	}

	glVertex2f(cenx, ceny);

	glEnd();

	free(rp);
}

void
draw_curtain()
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(0.2f, 0.2f, 0.2f, 0.75f);
	glRectf(video.x, video.y, video.w, video.h);
	glDisable(GL_BLEND);
}

void 
draw_inspecting_ball()
{
	glEnable(GL_DEPTH_TEST);
	set_color(WHITE);
	glPushMatrix();
		glTranslatef(inspect.org.x, inspect.org.y, 0.0f);
		draw_solid_sphere(inspect.r);
	glPopMatrix();
	glDisable(GL_DEPTH_TEST);
}

void
draw_inspecting_point()
{
	set_color(LUKE_BLACK);
	glLineWidth(3.0);
	glPushMatrix();
		glTranslatef(inspect.org.x - hit.x, 
			     inspect.org.y - hit.y, 
			     0.0);
		glBegin(GL_LINE_LOOP);
		draw_sphere(inspect.r / 10.0);
		glEnd();
	glPopMatrix();
}

void 
draw_inspect()
{
	draw_curtain();	
	draw_inspecting_ball();
	draw_inspecting_point();
}

void 
draw_sphere(double r)
{
	int i, step;
	double rang, alpha;

	if 	(r < 10.0)
		step = 10;
	else if (r < 30.0)
		step = 30;
	else
		step = 50;
	
	rang = 2.0f * M_PI / step;

	for (i = 0; i < step; ++i) {
		alpha = rang * i;
		glVertex2f(r * sin(alpha), r * cos(alpha));
	}
}

void 
draw_solid_sphere(double r)
{
	glBegin(GL_POLYGON);
	draw_sphere(r);
	glEnd();
}

void 
draw_ball(struct ball orb, enum color co)
{
	glPushMatrix();
		glTranslatef(orb.pos.x, orb.pos.y, 0.0f);
		set_color(SHADOW);
		draw_solid_sphere(radius + shadow);
		set_color(co);
		draw_solid_sphere(radius);
		set_color(ROSEWATER);
		draw_solid_sphere(radius/4.0);
	glPopMatrix();
}

void 
set_color(enum color co)
{
	int i, hexd = co;
	double res[3];
	for (i = 0; i < 3; ++i) {
		res[i] = (double)(hexd % 0x100) / 255.0f;
		hexd /= 0x100;
	}

	glColor4f(res[2], res[1], res[0], 1.0f);
}

void
cushion_shadow(struct rect field)
{
	struct rect shade;
	double *rp;

	shade = zoom_rect(field, -shadow);

	rp = sparse_rect_all_edges(shade);
	if (!rp) 
		drawfatal("Unable to sparse rectangle!\n");

	set_color(SHADOW);
	glBegin(GL_POLYGON);

	glVertex2f(rp[0], rp[4]);
	glVertex2f(rp[1], rp[5]);
	glVertex2f(rp[1] + shadow, rp[5] - shadow);
	glVertex2f(rp[0] + shadow, rp[4] - shadow);

	glEnd();

	glBegin(GL_POLYGON);

	glVertex2f(rp[2], rp[6]);
	glVertex2f(rp[3], rp[7]);
	glVertex2f(rp[3] - shadow, rp[7] + shadow);
	glVertex2f(rp[2] - shadow, rp[6] + shadow);

	glEnd();

	free(rp);
}

/*
 * Draw a table with rail and cushion width
 */
void
draw_table(struct rect field, double wrail, double wcush)
{
	struct rect rail, cushion;
	
	rail = zoom_rect(field, wrail + wcush);
	cushion = zoom_rect(field, wcush);

	set_color(DARK_RED);
	draw_rounded_rect(rail, 1.5);

	set_color(TEAL);
	glRectf(cushion.ll.x, cushion.ll.y, cushion.ur.x, cushion.ur.y);

	set_color(TEAL);
	glRectf(field.ll.x, field.ll.y, field.ur.x, field.ur.y);

	cushion_shadow(field);
}

void 
cls()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void 
redraw()
{
	double factor, rail, cushion;

	if (game_type == BLANK)
		drawfatal("Please initialize game!\n");

	factor = 7.1;
	rail = 1.9;
	cushion = 0.45;

	/* Clear screen */
	cls();
	glLineWidth(2.0);

	glPushMatrix();
		glScalef(factor, factor, 1.0f);

		glPushMatrix();

		glTranslatef(rail + cushion, rail + cushion, 0.0f);
		draw_table(field, rail, cushion);

		draw_ball(cue_ball, WHITE);
		draw_ball(obj_ball[0], YELLOW);
		draw_ball(obj_ball[1], RED);

		if (!is_motion) 
			draw_cue_aim(GREY);

		glPopMatrix();

	glPopMatrix();

	if (inspect.drawing) {
		draw_inspect();
	}

	glFlush();
}

void 
draw_cue_aim(enum color co)
{
	struct vec2 collid = aim_cue();

	set_color(co);

	glLineStipple(3, 0xAAAA);
	glEnable(GL_LINE_STIPPLE);

	glBegin(GL_LINE_STRIP);
	glVertex2f(cue_ball.pos.x, cue_ball.pos.y);
	glVertex2f(collid.x, collid.y);
	glEnd();

	glPushMatrix();
		glTranslatef(collid.x, collid.y, 0.0f);
		glBegin(GL_LINE_STRIP);
		draw_sphere(radius);
		glEnd();
	glPopMatrix();

	glDisable(GL_LINE_STIPPLE);
}

void 
reshape_viewport(unsigned int w, unsigned int h)
{
	glViewport(video.x, video.y, w, h);
	video.w = w;
	video.h = h;
}

void 
initialize_gl()
{
	glShadeModel(GL_FLAT);
	glLineWidth(2.0);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glOrtho(video.x, video.w, video.y, video.h, -99999, 99999);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void
initialize_grape()
{
	is_motion = 0;
	inspect.drawing = 0;

	hit.x = 0.0;
	hit.y = 0.0;

	initialize_gl();

	inspect.org.x = video.w / 2;
	inspect.org.y = video.h / 2;

	inspect.r = MIN(inspect.org.x, inspect.org.y) / 2;
}

void 
inspecting()
{
	if (inspect.drawing)
		inspect.drawing = 0;
	else 
		inspect.drawing = 1;
}

void
adjust_inspect(double x, double y)
{
	struct vec2 res;
	double r;

	res.x = hit.x + x;
	res.y = hit.y + y;
	r = inspect.r * (9.0 / 10.0);

	if (!inspect.drawing)
		return;
	if (SQU(r) - SQU(res.x) < 0)
		return;
	if (SQU(r) - SQU(res.y) < 0)
		return;
	if (SQU(res.x) + SQU(res.y) >= SQU(r))
		return;

	hit = res;
}

void
hitting()
{
	double r, ang;
	inspect.drawing = 0;

	if (ZEROF(hit.x)) {
		ang = hit.y < EPS ? M_PI / 2 : 3 * M_PI / 2;
	} else if (ZEROF(hit.y)) {
		ang = hit.x < EPS ? M_PI : 0.0;
	} else {
		ang = atan(hit.x / hit.y);
	}
	r = radius * hypot(hit.x, hit.y) / inspect.r;
	
	strike_cue_ball(40.0, ang, r);
}

void
roll_cue_clockly()
{
	if (!inspect.drawing)
		adjust_cue(-10 * M_PI / 500);
}

void
roll_cue_counterclockly()
{
	if(!inspect.drawing)
		adjust_cue(10 * M_PI / 500);
}

void
adjust_cue_clockly()
{
	if (!inspect.drawing)
		adjust_cue(-10 * M_PI / 4000);
}

void
adjust_cue_counterclockly()
{
	if (!inspect.drawing)
		adjust_cue(10 * M_PI / 4000);
}
