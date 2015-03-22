/* file: random.c
   $Id: random.c,v 1.1 2005/01/19 14:39:58 baker Exp $
   author: Ted Baker

   random number generation tools

   Your simulator should not rely on any details contained
   in this file, since this is just one of several implementations
   of these functions that might be produced, in order to
   vary the behavior of the simulation.

   Just rely on the interface defined in the file random.h.
   
 */

#define _XOPEN_SOURCE 500
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "random.h"

/* we know this is not a realistic addressing model;
   it is just one that is easy to compute */

typedef struct paging_model {
  /* first code page is zero */
  int code_size;
  /* last code page is code_size - 1 */
  /* first data page is code_size */
  int data_size;
  /* last data page is code_size + data_size - 1 */
  /* first stack page is code_size + data_size */
  int code_location;
  int stack_top;
} paging_model_t;

int nprocs      =    2; /* number of process arrivals */
int page_size   = 4096; 
int vmem_pages  = 1024; /* pages in virtual address space */
int real_pages  =   64; /* pages in real memory */
int disk_time   =  100; /* time for disk i/o operation */
double mean_interarrival_time = 10.0;
double mean_execution_time    = 20.0;
double mean_code_size         =  8.0;
double mean_data_size         =  8.0;
double p_jump   =  0.1;
double p_call   =  0.1;

int
random_exp_dist (double m) {
  int i;
  double d;
  /* drand48 returns value in range [0.0, 1.0] */
  d = -log (drand48()) * m;
  if (d > (double) RAND_MAX) {
    return RAND_MAX;
  }
  i = (int) d;
  if (((double) i) < d) return i + 1;
  return i;
}

int
random_interarrival_time () {
  return random_exp_dist (mean_interarrival_time);
}

int
random_execution_time () {
  return random_exp_dist (mean_execution_time);
}

int
random_page_reference (random_paging_model_t model) {
  /* this is a bare-bones version;
     a more realistic model will be provided later */
  int result;
  paging_model_t *m = (paging_model_t *) model;
  result = (int)(drand48() * (m->code_size + m->data_size)) % vmem_pages;
  return result;
}
  
random_paging_model_t
random_new_paging_model() {
  paging_model_t * t =
    (paging_model_t *) malloc (sizeof (paging_model_t));
  t->code_size = random_exp_dist (mean_code_size);
  t->data_size = random_exp_dist (mean_data_size);
  t->stack_top = t->code_size + t->data_size; 
  t->code_location = 0;
  return (void *) t;
}

void
random_initialize (int argc, const char *argv[]) {
  command_check_args (argc, argv,
                      &nprocs, "-nprocs=", "%d");
  command_check_args (argc, argv,
                      &mean_interarrival_time, "-miat=", "%lf");
  command_check_args (argc, argv,
                      &mean_execution_time, "-mexe=", "%lf");
  command_check_args (argc, argv,
                      &page_size, "-page_size=", "%d");
  command_check_args (argc, argv,
                      &vmem_pages, "-vmem=", "%d");
  command_check_args (argc, argv,
                      &real_pages, "-real=", "%d");
  command_check_args (argc, argv,
                      &disk_time, "-disk=", "%d");
  command_check_args (argc, argv,
                      &mean_code_size, "-code=", "%lf");
  command_check_args (argc, argv,
                      &mean_data_size, "-data=", "%lf");
}  

int debugging_level = 8;
/* zero means no debugging output
   larger values mean larger amounts of output */

void
debug_initialize (int argc, const char *argv[]) {
  command_check_args (argc, argv,
		      &debugging_level, "-debug=", "%d");
}

void
command_check_args (int argc, const char *argv[],
		    void *var, const char *name, const char *format) {
  int i;
  char * c;
  for (i = 1; i < argc; i++) {
    if (!(c = strpbrk (argv[i], "="))) continue;
    c++;    /* skip over the "=" */
    if (! strncmp (argv[i], name, sizeof(name) - 1)) {
      if (sscanf (c, format, var) != 1)
	fprintf (stderr, "usage: %s\n", argv[i]);
      break;
    }
  }
}
