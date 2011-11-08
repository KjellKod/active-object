/**
* 2010 by KjellKod.cc. This is PUBLIC DOMAIN to use at your own risk.
* No warranties whatsoever.
*
* Example of a Active Object, using Qt thread mechanisms to make it
* safe for thread communication.
*
* This was originally published at http://sites.google.com/site/kjellhedstrom2/active-object-with-cpp0x
* and heavily influenced from Herb Sutter's C++0x Active Object
* http://herbsutter.com/2010/07/12/effective-concurrency-prefer-using-active-objects-instead-of-naked-threads/
*/


#include "activeqthread.h"

// Factory: safe construction of object before thread start
std::auto_ptr<ActiveQThread> ActiveQThread::createActiveQThread()
{
  std::auto_ptr<ActiveQThread> active(new ActiveQThread);
  active->start();  // start the thread
  return active;
}


// Graceful quit of thread, first empty queue
ActiveQThread::~ActiveQThread()
{
  ActiveCallback quitMsg = std::tr1::bind(&ActiveQThread::doDone, this);
  send(quitMsg);
  wait(); // wait for queue to drain
}


// API to put background jobs onto the queue
void ActiveQThread::send(ActiveCallback msg)
{
  job_queue_.push(msg);
}

/// Private will only be called through factory function
ActiveQThread::ActiveQThread() : QThread() // no parent for thread, it is managed HERE
  , done_(false) {}

// Will wait indefinitely if needed for messages (if queue is empty)
// when background jobs are pushed onto the queue the thread is notified and the WaitCondition is fullfilled
// and the thread is woken up again
void ActiveQThread::run()
{
  while (!done_)
  {
    ActiveCallback func;
    job_queue_.wait_and_pop(func);
    func(); // executing queued job in this background thread
  }
}


/// "EndMessage" job, must only be called by destructor
void ActiveQThread::doDone()
{
  done_ = true;
}

