/* file: random.h
   $Id: random.h,v 1.1 2005/01/19 14:39:58 baker Exp $
   author: Ted Baker

   interface to random number generation tools
   
   If you use this, you should probably also use srand48() to set
   a nontrival value for the seed of the random number generator.

*/

#ifndef RANDOM_H
#define RANDOM_H

#include <stdlib.h>
#include <stdio.h>

/* The following simulation parameters have the specified
   default values.  They may be overridden by assigning them
   new values. Provision for such overiding is provided in
   the function random_initialize(). */

extern int nprocs;      /* number of processes */
                        /* default =    2; */
extern int page_size;   /* number of addresses per page */
                        /* default = 4096; */
extern int vmem_pages;  /* pages in virtual address space */
                        /* default = 1024; */
extern int real_pages;  /* pages in real memory */
                        /* default =   64; */
extern int disk_time;   /* time for disk i/o operation */
                        /* default =  100; */ 
extern double mean_interarrival_time;
                        /* default = 10.0; */
extern double mean_execution_time;
                        /* default = 20.0; */

int random_exp_dist (double m);
/* returns a random value in the range 1 .. RAND_MAX
   with approximately the inverse exponential distribution
   with mean value m.
         A(x) = 1 - e ** (-1/m)
*/

int random_interarrival_time ();
/* returns a non-negative value with mean
   corresponding to the value of mean_interarrival_time */

int random_execution_time ();
/* returns a non-negative value with mean
   corresponding to the value of mean_execution_time */

typedef void * random_paging_model_t;
/* opaque type representing a page reference model */

random_paging_model_t random_new_paging_model ();
/* returns a reference to a new paging model object;
   this should be called once for each process */

int random_page_reference (random_paging_model_t model);
/* returns a random number in the range 0 .. vmem_pages;
   this should be called to generate the next page reference
   for the process corresponding to model */
   
void random_initialize(int argc, const char *argv[]);
/* sets configurable variables from command line arguments
   and performs other initialization of global variables;
   this should be called once, from the main program,
   before any of the other functions defined here */

extern int debugging_level;
/* zero means no debugging output
   larger values mean larger amounts of output */

#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>

#ifdef DEBUG
#define TRACE(MSG, VAL, LEVEL) \
  {if ((LEVEL) <= debugging_level) \
  fprintf (stderr, "TRACE: %s (%d)\n", MSG, (int)(VAL));}
#else
#define TRACE(MSG, VAL, LEVEL)
#endif

void debug_initialize (int argc, const char *argv[]);

void command_check_args
(int argc,
 const char *argv[],
 void *var,           /* the variable to receive the value */
 const char *name,    /* the string to mark the argument */
 const char *format); /* the format to read in the value */

#endif

