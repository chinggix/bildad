#include <stdio.h>

#include "xws.h"
#include "draw.h"
#include "game.h"

int 
main(int argc, char **argv)
{  
  initialize_x();
  setup_xevent(reshape_viewport);
  initialize_gl();
  setup_carom3c();

  return 0;
}
