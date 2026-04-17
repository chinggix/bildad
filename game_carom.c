
#include "cartes.h"
#include "game.h"

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
void
setup_carom3c()
{
  double sight;
  field.ll.x = 0.0; field.ll.y = 0.0;
  field.ur.x = 50.0; field.ur.y = 100.0;
  
  sight = rect_width(field) / 4.0;
  obj_num = 2;
  
  obj_ball[0].pos.x = 2 * sight;
  obj_ball[0].pos.y = 2 * sight;

  obj_ball[1].pos.x = 2 * sight;
  obj_ball[1].pos.y = 6 * sight;

  cue_ball.pos.x = 1.5 * sight;
  cue_ball.pos.y = 2.0 * sight;
}

