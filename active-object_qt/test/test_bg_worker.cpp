/* *****************************************************************
Test of common usage of active object.

FYI: Active object is an object with a background thread incapsulated.
     Here! it has a public function API for giving jobs, asynchronously
     to the background thread. The jobs are enqueued and the thread
     work off the queue and it's jobs.

     Normally a job is a callback, in the background thread's runtime to
     the private function API of the object. This gives a separation of
     caller thread and background thread that is race and deadlock free
     as long as they do not share data between public and private API.

     I.e. public API is NEVER allowed to touch any of the internals of the
     object apart from adding jobs to the queue.


Tests below:
    1. Send jobs through public API and verify that they are done.

    2. -||- verify that they're done by another thread than the caller thread

    3. Let the caller thread add a background job, then at some later point WAIT
       for that job to be processed before it continues (wait condition), i.e.
       a normal synchronization scheme.

    4. Let the caller thread add a background job, then at some later point WAIT
       for that job to be processed and read it's return value.

       Please not that this would NORMALLY be done with a 'future' or continues
       futures but here we want to use 'normal' c++ so we give a return shared_queue
       --- just for proof of concept ---
       In the normal case (for example a driver the keeps pumping data up) you would
       normally wring your own thing with 'fixed/coded' input/output queues


       Information that is outside the scope of this active object
       ===========================================================
     *)If using the QtConcurrent framework:
     Using a Future value as a return value by the background thread
     the value can be read when the job is processed. For more information
     about QtConcurrent (run for example: http://doc.trolltech.com/latest/threads-qtconcurrent.html)
     and "Futures" please see: http://doc.qt.nokia.com/latest/qfuture.html
     and FutureWatcher: http://doc.trolltech.com/latest/qfuturewatcher.html

 See also this example that shows how QFuture and QFutureWatcher can be used
 to signal back (to the right caller thread) that it is finished
 http://codejourneys.blogspot.com/2008/06/qt-simple-example-of-use-of.html




*************************************************************** */

#include <gtest/gtest.h>

#include <memory>
#include <ctime>
#include <cmath>
#include <unistd.h>
#include <sys/time.h>


#include <QMutex>
#include <QWaitCondition>
#include <QTime>
//#include <QFuture>
//#include <QFutureWatcher>

#include "activeqthread.h"



// Please note that the background thread function API and members are PROTECTED
// and not PRIVATE. This is only for test purposes, normally the
// API should always be PRIVATE
class Test_ThreadCommunication: public ::testing::Test {
  public:
    // tell background thread to add this value to "addition_" member
    void add(const int value) {
      worker_->send(std::tr1::bind(&Test_ThreadCommunication::bgAdd, this, value));
    }

    // tell the bg thread to save it's id
    void saveBgThreadId() {
      worker_->send(std::tr1::bind(&Test_ThreadCommunication::bgSaveId, this));
    }


    // setup thread to notify when finished with the input task
    void workThenNotifyWhenFinished(int wait_ms, QWaitCondition *notifier) {
      // option you can make a private message and "bake-in" the wait_ms with the QWaitCondition
      // HERE, instead we put them both straight to the bg function
      worker_->send(std::tr1::bind(&Test_ThreadCommunication::bgNotifyWhenDone, this, wait_ms, notifier));
    }


    void calculateAndReturn(const int value,const int multiply, shared_queue<int>* result_queue)
    {
        worker_->send(std::tr1::bind(&Test_ThreadCommunication::bgCalculateAndReturn, this, value, multiply, result_queue));
    }



  protected:
    virtual void SetUp() {
      worker_ = std::auto_ptr<ActiveQThread>(ActiveQThread::createActiveQThread());
      addition_ = 0;
      bg_thread_id_ = 0;
    }


    /// normally usage should be private, but for test purposes made protected
    std::auto_ptr<ActiveQThread> worker_;
    int addition_;
    unsigned long bg_thread_id_;

  private:
    // Background thread API:s private function that must only
    // be accessed from active_object thread
    void bgAdd(int value) {
      addition_ += value;
    }
    // save bg thread id
    void bgSaveId() {
      bg_thread_id_ = static_cast<unsigned long>(QThread::currentThreadId());
    }
    // do work (sleep) then notify when finished
    void bgNotifyWhenDone(int work_time_ms, QWaitCondition *notifier) {
      usleep(work_time_ms * 1000);
      notifier->wakeOne(); // wake up waiting main thread
    }
    // Here maybe it would've been smarter/prettier to bake in all the arguments into a single message struct?
    // Calculate and send back through output queue
    void bgCalculateAndReturn(const int value,const int  multiply,shared_queue<int>* result_queue)
   {
        int result = value * multiply;
        result_queue->push(result);
    }

};




// At initialization the worker's active object should be created
TEST_F(Test_ThreadCommunication, initialized) {
  ASSERT_TRUE(worker_.get() != NULL);
}


// Verify that jobs can be done (in the background)
// also by printouts only verify that it is different
// threads that do the calculation as the thread that
// adds the calculation jobs
TEST_F(Test_ThreadCommunication, bg_calculations) {
  const int start = 0;
  const int stop = 1000;
  int check = 0;
  for (int i = start; i < stop; ++i) {
    check += i;  // for comparing with bg calculations
    add(i);      // calculatge int the background
  }
  // delete (i.e. drain the queue) to be sure all values are added
  worker_.reset();
  ASSERT_EQ(check, addition_);
}


// Verify that thread_id is different from the creator id
// i.e. we're running two threads.
TEST_F(Test_ThreadCommunication, different_threads) {
  unsigned long main_thread_id = static_cast<unsigned long>(QThread::currentThreadId());

  ASSERT_TRUE(bg_thread_id_ == 0);  // default zero
  saveBgThreadId(); // save thread_id in the background;
  worker_.reset(); // wait for msg to be processed
  ASSERT_NE(bg_thread_id_, 0); // id now updated
  ASSERT_NE(bg_thread_id_, main_thread_id);
}


// This thread verifies that it's possible for the calling thread to wait/sleep
// until the background thread has finished with it's job.
//
// This is normally only done if it's needed to have a synchronization point
// , wait point so that normal computation can continue after the background
// job was finished.
//
// For background computed values that should be returned see to QFuture usage
//
// This test also verifies that the background thread IS actually taking at least
// the demanded time AND that the calling thread is sleeping/waiting until the
// background thread wake's it up
//
// Please note that QTimer is the ONLY way (until C++0x) to measure
// the time in a non-os dependent way --- see also this to understand
// why clock() cannot be used:
// http://stackoverflow.com/questions/588307/c-obtaining-milliseconds-time-on-linux-clock-doesnt-seem-to-work-properly
TEST_F(Test_ThreadCommunication, wait_and_notify) {
  QWaitCondition processed_finished;
  QMutex waiting;
  QTime stopwatch;

  // start measure time
  stopwatch.start();

  // let bg thread work for given time, then tell us through
  // the wait condition that it's finished
  const int wait_ms = 1000; // 1s
  waiting.lock();
  workThenNotifyWhenFinished(wait_ms, &processed_finished);

  // silly check that verifies that asynchronous call was just that, asynchronous and fast
  int post_asynchronous_call_ms = stopwatch.elapsed();


  // Wait/sleep till bg thread notifies us
  processed_finished.wait(&waiting); // Zzzz
  int post_wakeup_ms = stopwatch.elapsed(); // Awake
  waiting.unlock(); // Release the mutex for any other thread that wants to use it (here: no one)


  std::cout << "\t\t\tIn [ms]. Minimum expected wait [" << wait_ms << "]";
  std::cout << ", actual wait [" << post_wakeup_ms << "]" << std::endl;
  std::cout << "\t\t\t(asynchronous call took: [" << post_asynchronous_call_ms << "])" << std::endl;

  ASSERT_TRUE(post_asynchronous_call_ms < wait_ms);
  ASSERT_TRUE(post_wakeup_ms >= wait_ms);
}


// A really silly thing, that just sends integers and receives them
// back with a factor multiplied. Just to show that it's no biggie to
// send values back to the caller.
//
// This actually show's also how to implement a kind of rudimentary future :)
// and yes,. maybe a bit overkill to "send" a whole queue ;) but it's only a pointer
TEST_F(Test_ThreadCommunication, work_and_return_by_queue) {

  shared_queue<int> result_queue;
  const int factor = 2;

  int maxTime = 0;
  int minTime = 100;  // for printouts

  for (int i = 0; i < 100; ++i) {
    int check = i * factor;
    QTime stopwatch;
    stopwatch.start();

    calculateAndReturn(i, factor, &result_queue);
    int result;
    result_queue.wait_and_pop(result);
    if(!result_queue.empty())
    {
      ASSERT_EQ(result, check);
    }
    // Here do other whatever work and then when you absolutely need this value
    // (in case we want to pretend we are dealing with rudimentary future)
    int time = stopwatch.elapsed();
    if(time > maxTime)
        maxTime = time;
    if(time < minTime)
        minTime = time;
  }

  std::cout << "\t\t\t Minimum roundtrip took:" << minTime;
  std::cout << ", maximum took: " << maxTime << std::endl;

}
