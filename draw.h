#ifndef DRAW_H
#define DRAW_H

enum color {
	WHITE 	= 0xffffff,
	GREY	= 0xc7c7c7,
	YELLOW 	= 0xddee3d,
	RED 	= 0xff3333,
	LUKE_BLACK	= 0x2C3947,
	DARK_BLUE 	= 0x1a1953
};

void initialize_grape(void);
void reshape_viewport(unsigned int, unsigned int);
void inspecting(void);
void adjust_inspect(double x, double y);
void hitting(void);

void redraw(void);

extern struct vec2 hit;
extern int is_motion;

#endif /* DRAW_H */
