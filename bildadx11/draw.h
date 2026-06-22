#ifndef DRAW_H
#define DRAW_H

enum color {
	GREY		= 0xc7c7c7,
	WHITE 		= 0xEEEDED,
	YELLOW 		= 0xF0DE36,
	RED 		= 0xD71313,
	WOOD		= 0x81912F,
	SHADOW		= 0x853953,
	TEAL		= 0x179299,
	ROSEWATER	= 0xdc8a78,
	DARK_RED	= 0x8A244B,
	DARK_BLUE 	= 0x0D1282,
	LUKE_BLACK	= 0x2C3947,
	LIGHT_BLUE 	= 0xE8EDF2
};

void initialize_grape(void);
void reshape_viewport(unsigned int, unsigned int);
void inspecting(void);
void adjust_inspect(double x, double y);
void hitting(void);
void roll_cue_clockly(void);
void roll_cue_counterclockly(void);
void adjust_cue_clockly(void);
void adjust_cue_counterclockly(void);

void redraw(void);

extern struct vec2 hit;
extern int is_motion;

#endif /* DRAW_H */
