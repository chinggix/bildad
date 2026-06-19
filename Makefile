#------------------------
# Consider changing these thing
# Paths
X11INC = /usr/include/X11
X11LIB = /usr/lib/X11

# includes and libs
INCS = -I$(X11INC)
LIBS = -L$(X11LIB) -lm -lrt -lX11 -lutil -lGL

# flags
STCFLAGS = $(INCS) $(CPPFLAGS) $(CFLAGS)
STLDFLAGS = $(LIBS) $(LDFLAGS)

# compiler and linker
CC = cc

#------------------------
SRC = bildad.c game_carom.c xws.c cartes.c draw.c game.c
OBJ = $(SRC:.c=.o)

all: bildad

.c.o:
	$(CC) $(STCFLAGS) -c $<

game.o: game.h
game_carom.h: game.h
cartes.o: cartes.h
xws.o: xws.h
draw.o: draw.h

bildad: $(OBJ)
	$(CC) -o $@ $(OBJ) $(STLDFLAGS)

clear: clean

clean: 
	rm -rf $(OBJ)
	rm -rf bildad

.PHONY: all clear clean

