// KjellKod header file ---------------------------------//
//
// (C) Copyright Kjell Hedström 2009
// Distributed under the KjellKod Software license,
// see (accompanying file) LICENSE_1_0.txt or copy at
// http://license.kjellkod.cc
//
// The software license grants permission without
// fee to copy, use and modify the software for any use
// (commercial and non-commercial)
// ------------------------------------------------------
// Code originally from KjellKod.cc by influence of
// KSignals and Active Objects (www.kjellkod.cc)
// ...
// ...
// See also Herb Sutter's article on solving this with C++0x
// "Prefer Using Active Objects Instead of Naked Threads"
// http://herbsutter.com/2010/07/12/effective-concurrency-prefer-using-active-objects-instead-of-naked-threads/


// .....................................................................
// README
// Important information: There ARE much better queues, dynamic ones
// that can be used. However since I'm using this as a barebone example
// but still want to point out how it would work with a smart pointer
// I have chosen to use my static size circular fifo with auto_ptr
//
// ... This is far from ideal but for the purpose of just showing
// it's OK. PLEASE replace the circularfifo and auto_ptr for better
// alternatives if you decide to use this code.
//
// 2010 Kjell Hedström,
// e-mail: hedstrom at kjellkod dot cc
// .....................................................................



#ifndef ACTIVE_H_
#define ACTIVE_H_

#include <pthread.h>
#include "circularfifo.h"
#include "activecallback.h"
#include <memory>

namespace kjellkod {

  // redicolusly small queue, to force "misses" and you to change queue :p
  static const int c_queueSize = 10000; 


  // IMPORTANT: Please replace this "msg_queue" for something else. 
  // It should be an INTERNALLY synchronized message queue that does not 
  // need EXTERNAL synchronization by the caller 
  // Why not a normal std::queue and protect it with one mutex?
  // ---- the CircularFifo is just put her for fun by me 
  // ---- 2010-08-12 Kjell 
  typedef CircularFifo<std::auto_ptr<Callback>, c_queueSize> msg_queue;


  class Active {
  private:  
    msg_queue mq;
    pthread_t thd;
    bool done;
    
    Active(); 
    Active(const Active&);
    void operator=(const Active&);

    void doDone(){done = true;} 
    void run();            
    static void* runThread(void* args); 


    public:
      virtual ~Active();     
      bool send(std::auto_ptr<Callback> msg_);
    
     // Factory: safe construction & thread start
     static std::auto_ptr<Active> createActive();    
};



} // end namespace kjellkod

#endif
