#ifndef GAME_H
#define GAME_H

#include "cartes.h"

#define MAX_OBJ 10
#define LEN(a) (sizeof(a) / sizeof((a)[0]))
#define FRICZ 3.82e-4
#define FRICR 3.80e-4

enum game { BLANK, CAROM };

struct ball {
	struct vec2 pos;
	struct vec2 vlo;
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
extern struct ball cue_ball;
extern unsigned int obj_num;
extern struct ball obj_ball[MAX_OBJ];
extern struct rect field;
extern struct cue stick;

struct vec2 aim_cue(void);
void rotate_cue(double);
void strike_cue_ball(double, double, double);

void setup_carom3c(void);

#endif /* GAME_H */
