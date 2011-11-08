/* Backgrounder.h
* Not any company's property but Public-Domain
* Do with source-code as you will. No requirement to keep this
* header if need to use it/change it/ or do whatever with it
*
* Note that there is No guarantee that this code will work
* and I take no responsibility for this code and any problems you
* might get if using it.
*
* 2010 @author Kjell Hedström, hedstrom@kjellkod.cc */


#ifndef BACKGROUNDER_H_
#define BACKGROUNDER_H_

#include <memory>
#include <vector>
#include <iostream>
#include <sys/time.h>

#include "active.h"
#include "activecallback.h"

namespace {
  /// Timer calculation (Ms)  
  double timeInMs(const timeval& begin_, const timeval& end_)  {
    double elapsedTime = (end_.tv_sec - begin_.tv_sec) *1000000 + (end_.tv_usec - begin_.tv_usec);
    return elapsedTime/1000; //us to ms
  }
}


/// Silly test background worker that only receives encapsuled integers and stores them in a vector
/// - the consumer receives "jobs" and sends them to a work queue to be asynchronously 
//    executed in FIFO order.
class Backgrounder {
 private:    
   /// Container for faking some imporant stuff type instead of an int
   /// so that it 'makes sense' storing it in an auto_ptr
   struct Data {
     Data(int v_) :value(v_) {}
     const int value;
   };

   static const int c_slowDownTimeUs = 500;  // fake time consuming job
   std::auto_ptr<kjellkod::Active> active;
   std::vector<int>& receivedQ;

   
   // background running functions
   void bgStoreData(const std::auto_ptr<Data> msg_); 
   void bgTimer(void);    
 
 public:
   explicit Backgrounder(std::vector<int>& saveQ_)
       : active(kjellkod::Active::createActive()), receivedQ(saveQ_){}
   virtual ~Backgrounder() {}

   // Asynchronous msg API
   bool saveData(const int value_); 
   bool timer(void); 
     
 };


// bg function, store data
void Backgrounder::bgStoreData(const std::auto_ptr<Data> msg_) {
  usleep(c_slowDownTimeUs);
  receivedQ.push_back(msg_->value);
}

// bg void arg function, print snapshot of size of queue
void Backgrounder::bgTimer(void) {
  static bool timeOn = false;
  static timeval t1;
  if(!timeOn){
    srand((unsigned) time(NULL));
    gettimeofday(&t1, NULL);    // timer start
    timeOn = true;
   } else {
     timeval t2;
     gettimeofday(&t2, NULL);
     std::ostringstream oss;
     oss << " (finished) in " << timeInMs(t1, t2) << " [ms]\t" << std::endl;
     std::cout << oss.str().c_str() << std::flush;
     timeOn = false;
   }
}


// Asynchronous API: Save an int (very silly)
// Limitation: Fixed size msg queue - if failed input returns false
bool Backgrounder::saveData(const int value_) {
  using namespace kjellkod;
  std::auto_ptr<Data> msg(new Data(value_));
  return (active->send(bind(this, &Backgrounder::bgStoreData, msg)));
}

// Asynchronous API: Start/Stop timer
// Limitation: Fixed size msg queue - if failed input returns false
bool Backgrounder::timer(void) {
  using namespace kjellkod;
  return (active->send(bind(this, &Backgrounder::bgTimer)));
}


#endif// Asynchronous msg API
