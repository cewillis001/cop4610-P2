/* file: sim.c
   $Id: sim.c,v 1.1 2005/01/19 14:39:58 baker Exp $
   author: Ted Baker

   The main program for a simulator.
   This was one of the programming assignments for
   COP 4610, in Spring 2002.

*/

#define _XOPEN_SOURCE 500
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "simulator.h"
#include "events.h"
#include "random.h"

/******
  main
 ******/

int
main(int argc, const char *argv[]) {

  random_initialize(argc, argv);
  debug_initialize(argc, argv);
  events_initialize();
  
  simulator_simulate(); 

  return 0;
}
