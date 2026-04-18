#ifndef DRAW_H
#define DRAW_H

#ifndef M_PI
#define M_PI 3.1415926
#endif

enum color {
  WHITE	      	= 0xffffff,
  YELLOW      	= 0xddee3d,
  RED		= 0xff3333,
  DARK_BLUE	= 0x1a1953
};
  
void initialize_gl(void);
void reshape_viewport(unsigned int, unsigned int);
void redraw(void);

#endif	/* DRAW_H */
