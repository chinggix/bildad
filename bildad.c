#include <stdio.h>

#include "draw.h"
#include "game.h"
#include "xws.h"

int main(void) {
  initialize_x();
  setup_xevent();
  initialize_gl();
  setup_carom3c();

  run_x();

  return 0;
}
