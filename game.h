#ifndef GAME_H
#define GAME_H

#include "cartes.h"

#define MAX_OBJ	10
#define LEN(a) 	(sizeof(a) / sizeof((a)[0]))

struct ball {
  struct vec2 pos;
  struct vec2 vlo;
  struct vec3 rot;
};

extern struct ball cue_ball;
extern struct ball obj_ball[];
extern unsigned int obj_num;
extern struct rect field;

void setup_carom3c(void);

#endif 	/* GAME_H */
