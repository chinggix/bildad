CFLAGS  = -ansi -Wall -g -Werror -pedantic -Os
LDFLAGS = -lm -lGL -lX11

SRC := main.c
OBJ := $(SRC:.c=.o)

bildad: $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

clear:
	rm -rf $(OBJ) bildad

.PHONY: clear
