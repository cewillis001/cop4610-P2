/* file: events.c
   $Id: events.c,v 1.1 2005/01/19 14:42:42 baker Exp baker $
   author: Ted Baker

   Does the simulation of individual events.

   This simulates the execution of a set of processes
   running with FIFO scheduling on a single CPU.

   This is intended mainly to provide an example of a simulator,
   and a starting point for development of a more complex
   simulator.

   It also illustrates the use of circular linked lists.
   These allow simpler insertions and deletions than null-terminated
   linked lists, but require that one work from the tail rather
   than the head.  That is, the head of the queue is accessed
   as the successor of the tail.  Two varieties are used:

      ready_queue  : doubly-linked
      process_list : singly-linked

   The ready_queue is doubly linked to make deletion from the
   middle possible in constant time.  We never delete anything
   from the process_list, so it is only singly linked.

   ----------------

   This version modified to replace explicit doubly linked 
   circular list for ready queue by similar structure of type
   struct list_head defined in "list.h". [T.P. Baker, January 2005]
   
 */

#define _XOPEN_SOURCE 500
#include <stdlib.h>
#include <stdio.h>
#include "simulator.h"
#include "random.h"
#include "list.h"

/*****************
  kinds of events
 *****************/

enum event_kinds {
  arrival_event,
  completion_event
} event_kind_t;

char * event_names [] = {
  "arrival_event    ",
  "completion_event ",
};

/********************
  process structures
 ********************/

typedef struct process { /* process control block */
  int id;
  simulator_time_t arrival_time;
  simulator_time_t execution_time;
  simulator_time_t completion_time;
  struct list_head ready_queue;
  struct process *next_in_list;
  simulator_action_t act;  /* to schedule event for this process */
  simulator_event_t event; /* to cancel event for this process */
  random_paging_model_t paging_model;
  /* add data here for virtual memory simulation here */
} process_t;

/****************
  CPU scheduling
 ****************/

process_t *idle_process;
/*  the idle process is also the tail of the read queue;
 *  it should never be deleted
 */

process_t *running_process;
simulator_time_t last_dispatching_time = 0;

void
queue_dump (char * msg, process_t *p) {
  process_t *t;
/*  fprintf (stdout, "%s:", msg); */
  fprintf (stdout, "%s:", "junk");
  list_for_each_entry(t, &idle_process->ready_queue, ready_queue)
    fprintf (stdout, "%d [%x]", t->id, (int) t);
  fprintf (stdout, "\n");
}  

#define ready_queue_first (list_entry(idle_process->ready_queue.next,process_t,ready_queue))

void
dispatch () {
  /* simulate process dispatching using FIFO policy */
  simulator_time_t d;
  d = now - last_dispatching_time;
  last_dispatching_time = now;
  running_process->execution_time -= d;
  if (running_process != ready_queue_first) {
    /* we need to switch processes */
    running_process = ready_queue_first;
    if (running_process != idle_process) {
      running_process->act.kind = completion_event;
      running_process->event =
       simulator_schedule (&(running_process->act),
			   now + running_process->execution_time);
    }
  }
}

/******************
  process creation
 ******************/

int process_count = 0;
/* the number of processes created so far
   and the id of the next proc to be created */
process_t *process_list;
/* tail of a circular list of all processes we have seen,
   linked by next_in_list */

void events_simulate (void * arg);

void
schedule_next_arrival() {
  process_t *p;
  p = (process_t *) malloc (sizeof (process_t));
  p->id = ++process_count;
  p->act.fun = events_simulate;
  p->act.arg = (void *) p;
  p->paging_model = random_new_paging_model();
  p->next_in_list = process_list->next_in_list;
  process_list->next_in_list = p;
  process_list = p;
  p->act.kind = arrival_event;
  p->event =
    simulator_schedule (&(p->act), now + random_interarrival_time());
  /* add code here to initialize any new per-process
     data that you add */
}

/************
  statistics
 ************/

void
print_report () {
  simulator_time_t sum = 0;
  simulator_time_t diff;
  process_t *p;
  fprintf (stdout, "-----------------------\n");
  for (p = process_list->next_in_list->next_in_list;
    p != idle_process;
    p = p->next_in_list)
  {
    diff = p->completion_time - p->arrival_time;
    fprintf (stdout, "proc %4d response time = %6d\n", p->id, diff);
    sum += diff;
  }
  fprintf (stdout, "-----------------------\n");
/*  fprintf (stdout, "average response time   = %6d\n", sum/nprocs); */
  fprintf (stdout, "average response time   = %6d\n", -1);
  fprintf (stdout, "completion_time         = %6d\n", now);
  fprintf (stdout, "total idle time         = %6d\n",
	   -idle_process->execution_time);
  fprintf (stdout, "percent idle time       = %6d%%\n",
	   -idle_process->execution_time * 100 / now);
  fprintf (stdout, "-----------------------\n");
}

/******************************
  main event simulation engine
 ******************************/

void
events_simulate (void *arg) {
  process_t *proc = (process_t *) arg;
  /* process associated with the event */
  TRACE ("->events_simulate", 0, 3);
  
  if (debugging_level > 1) {
    if (proc)
      fprintf (stderr, "       %s for proc %4d at time %6d\n",
       event_names[proc->act.kind], proc->id, now);
    else
      fprintf (stderr, "       %s at time %6d\n",
	event_names[proc->act.kind], now);
  }
  switch (proc->act.kind) {

  case arrival_event: {
    proc->arrival_time = now;
    proc->execution_time = now + random_execution_time();
    list_add_tail(&proc->ready_queue, &idle_process->ready_queue);
    assert (proc->execution_time >= 1);
    if (process_count < nprocs) schedule_next_arrival();
      /* schedule arrival of next process */
    break;
  }

  case completion_event: {
    proc->completion_time = now;
    list_del(&proc->ready_queue);
    break;
  }

  default: {
    /* should never get here */
    assert(0);
    break;
  }
  }
  dispatch();
  TRACE ("<-events_simulate", 0, 4);
}

/*******************************
  initialization & finalization
 *******************************/

void
events_initializer () {
  TRACE ("->events_initializer", 0, 3);
  assert (process_count == 0);
  assert (nprocs >= 1);
  /* initialize random number generator */
  srand (31459);
  if (!idle_process) {
    /* first time */
    idle_process = (process_t *) malloc (sizeof (struct process));
    idle_process->id = -1;
    idle_process->execution_time = 0;
    idle_process->next_in_list = idle_process;
    process_list = idle_process;
    /* make idle process the tail of the ready queue */
    INIT_LIST_HEAD(&idle_process->ready_queue);
    /* make the idle process the currently executing process */
    running_process = idle_process;
    /* add new code here to initialize any global data
       that you add */
  } else {
    fprintf (stderr, "attempt to repeat initialization\n");
    exit (-1);
  }  
  schedule_next_arrival();
  TRACE ("<-events_initializer", 0, 4);
}

void
events_finalizer () {
  TRACE ("->events_finalizer", 0, 3);
  print_report();
  TRACE ("<-events_finalizer", 0, 4);
}

void
events_initialize () {
  TRACE ("->events_initialize", 0, 3);
  simulator_register_handlers (events_initializer, events_finalizer);
  TRACE ("<-events_initialize", 0, 4);
}
