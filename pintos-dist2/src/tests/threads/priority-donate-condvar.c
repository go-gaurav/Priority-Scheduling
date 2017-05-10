/* Low priority thread L acquires two locks: lockA and lockB, then blocks doing cond_wait on lockA.
   Medium priority thread M then blocks waiting on lockB.
   Next, high priority thread H attempts to
   acquire the lockB, donating its priority to L.

   Next, the main signals the condition, waking up L.  L
   releases the lock, which wakes up H.  H signals the condition,
   waking up M.  H terminates, then M, then L, and finally the
   main thread.

   Written by Harman Singh <singhharm1@ecs.vuw.ac.nz>. */

#include <stdio.h>
#include "tests/threads/tests.h"
#include "threads/init.h"
#include "threads/synch.h"
#include "threads/thread.h"

struct lock_and_cond
  {
    struct lock lockA;
    struct condition condition;
    struct lock lockB;
  };

static thread_func l_thread_func;
static thread_func m_thread_func;
static thread_func h_thread_func;

void
test_priority_donate_condvar (void)
{
  struct lock_and_cond lc;

  /* This test does not work with the MLFQS. */
  ASSERT (!thread_mlfqs);

  /* Make sure our priority is the default. */
  ASSERT (thread_get_priority () == PRI_DEFAULT);


  lock_init (&lc.lockA);
  lock_init (&lc.lockB);
  cond_init (&lc.condition);
  thread_create ("low", PRI_DEFAULT + 1, l_thread_func, &lc);
  thread_create ("med", PRI_DEFAULT + 3, m_thread_func, &lc);
  thread_create ("high", PRI_DEFAULT + 5, h_thread_func, &lc);

  // main thread acquires lock and signals. The
  // now main thread signals and priority should be donated???
  lock_acquire(&lc.lockA);
  cond_signal(&lc.condition, &lc.lockA);
  msg ("Main thread finished.");
}

static void
l_thread_func (void *lc_)
{
  struct lock_and_cond *lc = lc_;

  lock_acquire(&lc->lockB);
  msg ("Thread L acquired lockB.");
  lock_acquire (&lc->lockA);
  msg ("Thread L acquired lockA.");
  cond_wait (&lc->condition, &lc->lockA);
  lock_release (&lc->lockA);
  lock_release(&lc->lockB);
  msg ("Thread L finished.");
}

static void
m_thread_func (void *lc_)
{
  struct lock_and_cond *lc = lc_;
  lock_acquire (&lc->lockA);
  msg ("Thread M acquired lockA.");
  cond_wait (&lc->condition, &lc->lockA);
  lock_release (&lc->lockA);
  msg ("Thread M finished.");
}

static void
h_thread_func (void *lc_)
{
  struct lock_and_cond *lc = lc_;

  lock_acquire (&lc->lockA);
  lock_acquire (&lc->lockB);
  msg ("Thread H acquired lockB.");

  cond_wait (&lc->condition, &lc->lockA);
  cond_signal(&lc->condition, &lc->lockA);
  lock_release (&lc->lockA);
  lock_release (&lc->lockB);
  msg ("Thread H finished.");
}
