#include <GL/gl.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

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
} video = {0.0f, 0.0f, 760.0f, 680.0f};

static void cls(void);
static void draw_table(struct rect);
static void set_color(enum color);
static void draw_sphere(double);
static void draw_solid_sphere(double);
static void draw_cue_aim(enum color);

void draw_sphere(double r) {
  int i, step;
  double rang, alpha;

  step = 10; /* Increase this for smooth sphere */
  rang = 2.0f * M_PI / step;

  for (i = 0; i < step; ++i) {
    alpha = rang * i;
    glVertex2f(r * sin(alpha), r * cos(alpha));
  }
}

void draw_solid_sphere(double r) {
  glBegin(GL_POLYGON);
  draw_sphere(r);
  glEnd();
}

void draw_ball(struct ball orb, enum color co) {
  set_color(co);
  glPushMatrix();
  glTranslatef(orb.pos.x, orb.pos.y, 0.0f);
  draw_solid_sphere(radius);
  glPopMatrix();
}

void set_color(enum color co) {
  int i, hexd = co;
  double res[3];
  for (i = 0; i < 3; ++i) {
    res[i] = (double)(hexd % 0x100) / 255.0f;
    hexd /= 0x100;
  }

  glColor3f(res[2], res[1], res[0]);
}

void draw_table(struct rect field) {
  set_color(DARK_BLUE);
  glRectf(field.ll.x, field.ll.y, field.ur.x, field.ur.y);
}

void cls() { glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); }

void redraw() {
  if (game_type == BLANK) {
    fprintf(stderr, "Please initialize game!\n");
    exit(1);
  }

  cls();

  glPushMatrix();
  glScalef(7.0f, 7.0f, 1.0f);

  draw_cue_aim(GREY);

  draw_ball(cue_ball, WHITE);
  draw_ball(obj_ball[0], YELLOW);
  draw_ball(obj_ball[1], RED);

  draw_table(field);

  glPopMatrix();
  glFlush();
}

void draw_cue_aim(enum color co) {
  struct vec2 collid = aim_cue();

  set_color(co);

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
}

void reshape_viewport(unsigned int w, unsigned int h) {
  glViewport(video.x, video.y, w, h);
  video.w = w;
  video.h = h;
}

void initialize_gl() {
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glLineStipple(3, 0xAAAA);
  glEnable(GL_LINE_STIPPLE);
  glLineWidth(2.0);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  glOrtho(video.x, video.w, video.y, video.h, -99999, 99999);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}
