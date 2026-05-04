#include "cartes.h"
#include "game.h"
#include <stdio.h>

/* Set up position like this:
 * ---------------
 * |		 |
 * |		 |
 * |	  o      |
 * |		 |
 * |		 |
 * |		 |
 * |		 |
 * |		 |
 * |	o o      |
 * |		 |
 * |		 |
 * ---------------
 * By default table is in portrait form
 */
void setup_carom3c()
{
	double sight;

	game_type = CAROM;

	field.ll.x = 0.0;
	field.ll.y = 0.0;
	field.ur.x = 50.0;
	field.ur.y = 100.0;

	sight = rect_width(field) / 4.0;
	radius = 1.128 * 1.15;
	mass = 0.4;
	obj_num = 2;

	obj_ball[0].pos.x = 2.0 * sight;
	obj_ball[0].pos.y = 2.0 * sight;

	obj_ball[1].pos.x = 2.0 * sight;
	obj_ball[1].pos.y = 6.0 * sight;

	cue_ball.pos.x = 1.5 * sight;
	cue_ball.pos.y = 2.0 * sight;

	stick.drc.x = 0.0;
	stick.drc.y = 1.0;
	stick.len = 4.5;
	stick.tip = 0.0035;
	stick.butt = 0.0045;

	halt_all_motion();
}

void 
halt_all_motion()
{
	MPTY2(obj_ball[0].vlo);
	MPTY3(obj_ball[0].rot);
	MPTY2(obj_ball[1].vlo);
	MPTY3(obj_ball[1].rot);
	MPTY2(cue_ball.vlo);
	MPTY3(cue_ball.rot);
}

void
halt_motion(struct ball *b)
{
	MPTY2(b->vlo);
	MPTY3(b->rot);
}
