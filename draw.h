#ifndef DRAW_H
#define DRAW_H

enum color {
	WHITE = 0xffffff,
	GREY = 0xc7c7c7,
	YELLOW = 0xddee3d,
	RED = 0xff3333,
	DARK_BLUE = 0x1a1953
};

void initialize_gl(void);
void reshape_viewport(unsigned int, unsigned int);

void redraw(void);

#endif /* DRAW_H */
