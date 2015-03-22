/* file: simulator.c
   $Id: simulator.c,v 1.3 2005/01/08 19:48:32 baker Exp baker $
   author: Ted Baker

   Body of simulation engine.

   Uses a null-terminated doubly-linked list for the event queue.
   
*/
#include <stdlib.h>
#include <stdio.h>

#include "simulator.h"
/* defines the interface to the simulation engine
 */
#include "random.h"

/*****************
  simulated clock
 *****************/

simulator_time_t now;
simulator_time_t next_event_time;

/*************
  event queue
 *************/

typedef struct event_queue_node * event_queue_link_t;
typedef struct event_queue_node {
  simulator_time_t   time;
  simulator_action_t *act;
  event_queue_link_t next;
  event_queue_link_t prev;
} event_queue_node_t;

event_queue_link_t event_queue_head = NULL;
event_queue_link_t free_events = NULL;

#ifdef DEBUG
void
simulator_queue_dump () {
  event_queue_link_t e = event_queue_head;
  fprintf (stdout, "QUEUE: ");
  while (e) {
     fprintf (stdout, "%d [%x]", e->time, (int) e->act->arg);
     e = e->next;
  }
  fprintf (stdout, "\n");
}
#endif

/********************
  simulator_schedule
 ********************/

simulator_event_t
simulator_schedule (simulator_action_t *act, simulator_time_t t) {
/* inserts event with give action into queue,
   keeping events in increasing order by time,
   and events of equal time in order by event comparison
 */
  event_queue_link_t prev = NULL;
  event_queue_link_t next = event_queue_head;
  event_queue_link_t event;
  /* allocate new event node */
  if (free_events) {
    event = free_events; free_events = free_events->next;
  } else {
    event = (event_queue_link_t) malloc (sizeof (event_queue_node_t));
  }
  /* initialize event data fields */
  event->time = t;
  event->act = act;
  /* find proper insertion point in queue */
  while (next && next->time <= t) {
    prev = next; next = next->next;
  }
  while (next && next->time == t && next->act->kind < act->kind) {
    prev = next; next = next->next;
  }
  /* insert between prev and next */
  if (prev) prev->next = event;
  else {
    event_queue_head = event;
    next_event_time = t;
  }
  if (next) next->prev = event;
  event->next = next;
  event->prev = prev;
  return event;
}

/******************
  simulator_cancel
 ******************/

void
simulator_cancel (simulator_event_t e){
  event_queue_link_t event = (event_queue_link_t) e;
  event_queue_link_t next, prev;
  assert (event != NULL);
  TRACE ("->cancel_event", event->time, 3);
  next = event->next; prev = event->prev;
  if (prev) prev->next = next;
  else {
    event_queue_head = next;
    if (next) next_event_time = next->time;
    else next_event_time = simulator_time_infinity;
  }
  if (next) next->prev = prev;
  event->next = free_events;
  free_events = event;
  TRACE ("<-cancel_event", event->time, 4);
}

/*****************************
  initializers and finalizers
 *****************************/

typedef struct handler_pair {
  void (*initializer)(void);
  void (*finalizer)(void);
  struct handler_pair * next;
} handler_pair_t;

handler_pair_t *handler_pair_list;

void
simulator_register_handlers
(void (*initializer)(void), void (*finalizer)(void)) {
  handler_pair_t *t =
    (handler_pair_t *) malloc (sizeof (handler_pair_t));
  t->initializer = initializer;
  t->finalizer = finalizer;
  t->next = handler_pair_list;
  handler_pair_list = t;
}

/********************
  simulator_simulate
 ********************/

void simulator_simulate () {
  event_queue_link_t current_event;
  handler_pair_t *h;
  TRACE ("->simulator_simulate", 1, 3);
  now = 0;
  for (h = handler_pair_list; h; h = h->next) {
    h->initializer();
  }
  TRACE ("--simulator_simulate", 2, 4);
  while (event_queue_head) {
     current_event = event_queue_head;
     event_queue_head = event_queue_head->next;
     if (event_queue_head) next_event_time = event_queue_head->time;
     else next_event_time = simulator_time_infinity;
     current_event->next = free_events;
     free_events = current_event;
     now = current_event->time;
     current_event->act->fun (current_event->act->arg);
  }
  TRACE ("--simulator_simulate", 5, 4);
  for (h = handler_pair_list; h; h = h->next) {
    h->finalizer();
  }
  TRACE ("<-simulator_simulate", 6, 4);
}


