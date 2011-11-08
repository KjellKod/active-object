
#include "bg_worker.h"
#include <gtest/gtest.h>

#include <memory>
#include <ctime>
#include <cmath>

#include <QMutex>
#include <QWaitCondition>
#include <QTime>

class Test_ThreadCommunication: public ::testing::Test {
  public:

  protected:
    virtual void SetUp() {
      worker_ = std::auto_ptr<BgWorker>(new BgWorker);
    }

    /** silly function that calculates in the
    * background lots of values.
    * Add all values together, increment of one from start to end
    * \param start of the counting
    * \param end of the counting
    * @return the finished BACKGROUND thread calculated result */
    int calculate_and_reset(int start, int stop);


    std::auto_ptr<BgWorker> worker_;
};


int Test_ThreadCommunication::calculate_and_reset(int start, int stop) {
  for (int i = start; i < stop; ++i) {
    worker_->addition(i);
  }
  // wait for queue, to drain get the result and restart worker from zero
  return worker_->reset_counter();
}






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
    check += i;
  }
  ASSERT_TRUE(check == calculate_and_reset(start, stop));
}


// verify that thread_id is different from the creator id
// i.e. we're running two threads.
TEST_F(Test_ThreadCommunication, different_threads) {
  worker_->save_thread_id(); // save thread_id in the background
  worker_->reset_counter();  // wait for the message queue to drain (id saved)
  EXPECT_NE(worker_->thread_id(), worker_->creator_thread_id());
  EXPECT_NE(0, worker_->thread_id());
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
  worker_->sleep_then_notify(wait_ms, &processed_finished);
  int post_asynchronous_call_ms = stopwatch.elapsed();

   // wait/sleep till bg thread notifies us
  processed_finished.wait(&waiting); // Zzzz
  int post_wakeup_ms = stopwatch.elapsed(); // Awake
  waiting.unlock(); // Release the mutex for any other thread that wants to use it (here: no one)


  std::cout << "\t\t\tIn [ms]. Minimum expected wait [" << wait_ms << "]";
  std::cout << ", actual wait [" << post_wakeup_ms << "]" << std::endl;
  std::cout << "\t\t\t(asynchronous call took: [" << post_asynchronous_call_ms << "])" << std::endl;

  ASSERT_TRUE(post_asynchronous_call_ms < wait_ms);
  ASSERT_TRUE(post_wakeup_ms >= wait_ms);
}









// TODO
// common scenarios
//
// 0. Send jobs that are done in the background
//
//
// 1. Validate that the JOB is done in different thread_id compared to the
//    caller
//
// 2. Let Caller WAIT until the bg_worker has finished the job
//
//
// 3. Let caller do other stuff and get notified with DATA
//    from the bg_thread when the job was finished
//
// 4. (similar to 2)
//    Let Caller WAIT until the bg_worker has finished the job
//    The Caller should then RECEIVE the data QFuture?
//
//

