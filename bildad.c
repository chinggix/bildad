#include <stdio.h>

#include "xws.h"
#include "draw.h"

int 
main(void)
{  
  initialize_x();
  setup_xevent(reshape_viewport);
  initialize_gl();

  return 1;
}
