/**
* 2010 by KjellKod.cc. This is PUBLIC DOMAIN to use at your own risk.
* No warranties whatsoever.
*
* Example of a normal std::queue protected by a mutex for operations,
* making it safe for thread communication.
*
* This exampel  was inspired by Anthony Williams lock-based data structures in
* Ref: "C++ Concurrency In Action" http://www.manning.com/williams */

#ifndef SHARED_QUEUE
#define SHARED_QUEUE

#include <queue>
#include <QMutexLocker>
#include <QMutex>
#include <QWaitCondition>

/** Note: updated after Anthony blogged about making this more efficient
* http://www.justsoftwaresolutions.co.uk/threading/implementing-a-thread-safe-queue-using-condition-variables.html
*
* Now safe and pretty effiecient even for multiple consumers */
template<typename T>
class shared_queue
{
  public:
    shared_queue() {}

    void push(T item_)
    {
      QMutexLocker lock(&mutex_);
      queue_.push(item_);
      lock.unlock();
      wait_condition_.wakeOne();
    }

    /// wait until a queue has item - safe for multiple consumers
    void wait_and_pop(T &item)
    {
      QMutexLocker lock(&mutex_);
      while (queue_.empty())
      {
        wait_condition_.wait(&mutex_);
      }
      item = queue_.front();
      queue_.pop();
    }

    /// try to get an item, returns true if successful
    bool try_pop(T &item_)
    {
      QMutexLocker lock(&mutex_);
      if (queue_.empty())
      {
        return false;
      }
      item_ = queue_.front();
      queue_.pop();
      return true;
    }

    bool empty() const
    {
      QMutexLocker lock(&mutex_);
      return queue_.empty();
    }

    unsigned size() const
    {
      QMutexLocker lock(&mutex_);
      return queue_.size();
    }

  private:
    std::queue<T> queue_;
    mutable QMutex mutex_;
    QWaitCondition wait_condition_;
    shared_queue &operator=(const shared_queue &);
    shared_queue(const shared_queue &other_);
};


#endif //SHARED_QUEUE
