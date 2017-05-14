/* Lab 2 : priority donation test using conditional variables
 *
   Low priority thread "Low" acquires two locks: lockA and lockB, then blocks doing cond_wait on lockA.
   Medium priority thread "Med" then also blocks waiting on lockA.
   Next, high priority thread "High" attempts to acquire the lockB,
   donating its priority to "Low".

   Next, the main signals the condition, waking up "Low".  "Low"
   releases lockB, which wakes up "High".  "High" signals the condition,
   waking up "Med".
   At this point all threads are unblocked.
   "High" terminates, then "Med", then "Low", and finally the
   main thread.

   This test is based on priority-donate-sema written by Godmar Back <gback@cs.vt.edu>.

   Written by Harman Singh <singhharm1@ecs.vuw.ac.nz>. */

#include <stdio.h>
#include "tests/threads/tests.h"
#include "threads/init.h"
#include "threads/synch.h"
#include "threads/thread.h"

struct lock lockA;
struct condition condition;
struct lock lockB;

static thread_func l_thread_func;
static thread_func m_thread_func;
static thread_func h_thread_func;

void
test_priority_donate_condvar (void)
{
  /* This test does not work with the MLFQS. */
  ASSERT (!thread_mlfqs);

  /* Make sure our priority is the default. */
  ASSERT (thread_get_priority () == PRI_DEFAULT);


  lock_init (&lockA);
  lock_init (&lockB);
  cond_init (&condition);
  msg("Thread Low : creating thread");
  thread_create ("low", PRI_DEFAULT + 1, l_thread_func, NULL);
  msg("Thread Med : creating thread");
  thread_create ("med", PRI_DEFAULT + 3, m_thread_func, NULL);
  msg("Thread High: creating thread");
  thread_create ("high", PRI_DEFAULT + 5, h_thread_func, NULL);

  // main thread acquires lock and signals. The
  // now main thread signals and priority should be donated???
  msg("Thread Main: signalling...");
  lock_acquire(&lockA);
  cond_signal(&condition, &lockA);
  lock_release(&lockA);
  msg ("Thread Main: finished.");
}

static void
l_thread_func (void *aux UNUSED)
{
  lock_acquire(&lockB);
  lock_acquire (&lockA);
  cond_wait (&condition, &lockA);
  msg ("Thread Low : is awake.");
  lock_release (&lockA);
  lock_release(&lockB);
  msg ("Thread Low : finished.");
}

static void
m_thread_func (void *aux UNUSED)
{
  lock_acquire (&lockA);
  cond_wait (&condition, &lockA);
  msg ("Thread Med : is awake.");
  lock_release (&lockA);
  msg ("Thread Med : finished.");
}

static void
h_thread_func (void *aux UNUSED)
{
  // high priority thread attempts to acquire lock held by lower priority thread. Thus donating its priority to lower priority thread
  lock_acquire (&lockB);
  msg ("Thread High: is awake.");

  lock_acquire (&lockA);
  cond_signal(&condition, &lockA);
  lock_release (&lockA);
  lock_release (&lockB);
  msg ("Thread High: finished.");
}
