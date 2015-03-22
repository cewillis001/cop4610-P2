/* file: simulator.h
   $Id: simulator.h,v 1.1 2005/01/19 14:39:58 baker Exp $
   author: Ted Baker

   Defines the visible interface of the simulator engine to the
   the code that simulates each event.

 */

#ifndef SIMULATOR_H

#include <limits.h>

#define simulator_time_infinity INT_MAX;
typedef int simulator_time_t;
extern simulator_time_t now;
/* the current value of the simulated clock */
extern simulator_time_t next_event_time;

typedef void * simulator_event_t;
/* an opaque type that is used identify a particular scheduled event;
   it should not be dereferenced or modified outside the simulator */

extern void
simulator_register_handlers
(void (*initializer)(void), void (*finalizer)(void));
/* registers a pair of initializer and a finalizer procedures;
   the initializer will be called before the start of a simulation;
   the finalizer will be called on completion of the simulation;
   to be useful for multiple simulation runs, these procedures
   should be idempotent (able to be called more than once safely). */

typedef struct simulator_action {
  void (*fun)(void *);
  void * arg;
  int kind;    /* used to order events with the same time */
} simulator_action_t;
/* specifies a call, fun(arg), to be made by the simulator
   at some future time */

extern simulator_event_t
simulator_schedule (simulator_action_t *act, simulator_time_t t);
/* schedules an event to peform action act at time t;
   it returns a value identifying this event;
   the value can be used with function cancel_event;
   when the time of the event arrives, the simulator calls
   action.fun (action.arg) */
  
extern void
simulator_cancel (simulator_event_t e);
/* cancels a previously scheduled event */

#ifdef DEBUG
extern void
simulator_queue_dump ();
/* lists contents of queue, for debugging only */
#else
#define simulator_queue_dump()
#endif

void
simulator_simulate ();
/* runs the simulation */

#endif
