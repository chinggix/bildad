#ifndef GAME_H
#define GAME_H

#include "cartes.h"

#define MAX_OBJ 10
#define LEN(a) 	(sizeof(a) / sizeof((a)[0]))
#define MUR	3.80e-4
#define MUX	MUR
#define MUY	MUR
#define MUZ	3.82e-4
#define MI	0.069
#define G0	9.80665		/* Earth gravitational constant */

enum game 	{ BLANK, CAROM };
enum contact    { BED, BALL, RAIL };

struct ball {
	struct vec2 pos;
	struct vec2 vlo;        /* Velocity at center           */
        struct vec2 vct;	/* Velocity at contact point    */
	struct vec3 rot;
};

struct cue {
	struct vec2 drc;
	double len;
	double tip;
	double butt;
};

extern enum game game_type;
extern double radius;
extern double mass;
extern struct ball cue_ball;
extern unsigned int obj_num;
extern struct ball obj_ball[MAX_OBJ];
extern struct rect field;
extern struct cue stick;

struct vec2 aim_cue(void);
void rotate_cue(double);
void strike_cue_ball(double, double, double);
int motion(void);

void setup_carom3c(void);

#endif /* GAME_H */
