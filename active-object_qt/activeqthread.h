#ifndef ACTIVEQTHREAD_H_
#define ACTIVEQTHREAD_H_
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


#include <QThread>
#include <QMutex>
#include <QWaitCondition>

#include <tr1/functional>
#include <memory>

#include "macro_definitions.h"
#include "shared_queue.h"


/// typedef to ease user syntax
typedef std::tr1::function<void()> ActiveCallback;

/**
* Removes 'bare bone' access to QThreads thereby minimizing thread
* problems such as internal race. Jobs are communicated by messages
* to internal background thread that waits if no jobs are available */
class ActiveQThread : public QThread {
    Q_OBJECT
  public:
    /** @return Active Object with internal background thread
     *  REMEMBER that the memory management of the object
     * is the responsibility of the caller */
    static std::auto_ptr<ActiveQThread>  createActiveQThread();

    /** Will add 'end processing job' at the end of the job queue
    * and wait for the queue to drain untill exiting. Thus ensuring that
    * all added jobs will be executed before deleting the object.
    *
    * I.e. The calling thread that invokes this destructor will wait for
    * the destructor (it's queue to drain and all background jobs to finish)  */
    virtual ~ActiveQThread();

  signals:
  public slots:
    /** The only public API for communicating with the internal thread
      * Jobs are added as function callbacks with or without coupled data
      * which is COPIED to job queue.
      *
      * The callback is executed on the internal background thread
      *
      * @param msg the function callback */
    void send(ActiveCallback msg);

  private:
    /// private constructor only reachable through @ref createActiveQThread
    explicit ActiveQThread();


    /** Thread loop, see @ref QThread::run.
    * The thread will wait/sleep if job queue is
    * empty, until new jobs or destructor (end job) is are added*/
    void run();

    /// "EndMessage" job, must only be called by destructor
    void doDone();

    /// mutex protexted shared queue
    shared_queue<ActiveCallback> job_queue_;
    bool done_;
    DISALLOW_COPY_AND_ASSIGN(ActiveQThread);
};


#endif // ACTIVEQTHREAD_H
