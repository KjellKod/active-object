// KjellKod header file ---------------------------------//
//
// (C) Copyright Kjell Hedström 2009
// Distributed under the KjellKod Software license,
// see (accompanying file) LICENSE10.txt or copy at
// http://license.kjellkod.cc
//
// The software license grants permission without
// fee to copy, use and modify the software for any use
// (commercial and non-commercial)
// ------------------------------------------------------
// Code originally from KjellKod.cc by influence of
// KSignals and Active Objects (www.kjellkod.cc)
// ...
// See also Herb Sutter's blog & article on solving this with C++0x
// "Prefer Using Active Objects Instead of Naked Threads"
// http://herbsutter.com/2010/07/12/effective-concurrency-prefer-using-active-objects-instead-of-naked-threads/
//


#include "active.h"
#include <cassert>
#include <iostream>

using namespace kjellkod;
Active::Active(): thd(NULL), done(false) {};


Active::~Active() {  
  // NOTE: Since our input work queue is a not a dynamic sized queue it CAN
  // become FULL, so just wait for the opening or better yet use a better&dynamic queue :p
  while(!send(bind(this, &Active::doDone)))
  {
     // single core help, ease off the CPU for bg thread (if on same core)
     pthread_yield(); 
   }
   pthread_join(thd, NULL); 
}



bool Active::send(std::auto_ptr<Callback> msg_) {
  return mq.push(msg_); // Add asynchronously to work queue

}



void Active::run() {
  while (!done) {    
    if (mq.isEmpty()) {
       pthread_yield();  // help switching if on same core
    } else {
       std::auto_ptr<Callback> msg;
       mq.pop(msg); 
       (*msg)();
    }
  }
}


void* Active::runThread(void* args) {
  Active* pActive = static_cast<Active*>(args);
  assert(NULL != pActive);
  pActive->run();
  pthread_exit(NULL);
}



std::auto_ptr<Active> Active::createActive() {
  std::auto_ptr<Active> active(new Active());
  
  int check = pthread_create(&(active->thd), NULL, runThread, active.get()); 
  assert((0 == check) && "Thread not created successfully!");
  return active;
}


