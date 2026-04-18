#include "game.h"

enum game 	 game_type = BLANK;
struct ball 	 cue_ball;
struct ball 	 obj_ball[MAX_OBJ];
unsigned int 	 obj_num;			/* Number of object balls */
double 		 radius;
struct rect	 field;				/* Playing field */
struct cue	 stick;
