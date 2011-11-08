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
//
// See also Herb Sutter's article on solving this with C++0x
// "Prefer Using Active Objects Instead of Naked Threads"
// http://herbsutter.com/2010/07/12/effective-concurrency-prefer-using-active-objects-instead-of-naked-threads/

//------------------------------------------------------------------------------
// Active Callback is encapsulation of function call together with
//          any data that should be used for the call. Very handy for making
//          asynchronous calls (using Active Object)

#ifndef ACTIVE_CALLBACK_H_
#define ACTIVE_CALLBACK_H_

#include <memory>

namespace kjellkod {

  /// Baseclass for callbacks. Gives easy storage in a container
  // without exposign the template type
  struct Callback {
    virtual void operator()() = 0;
    virtual ~Callback(){};
  };


  template<typename T, typename Param>
  class ActiveCallback: public Callback {
   private:
     typedef void (T::*Func)(Param);

     T*    object;
     Func func;
     Param param;
  
   public:
     ActiveCallback(T* obj_, void (T::*f_)(Param), Param& p_)
		: object(obj_), func(f_), param(p_) { } 

    virtual ~ActiveCallback() {}
    virtual void operator()() { (object->*func)(param); }     
  };



  template<typename T>
  class ActiveCallback<T, void>: public Callback {
   private:
    typedef void (T::*Func)(); // same as: "void (T::*method)(void)"
     
     T*    object;
     Func func;  

   public:
     ActiveCallback ( T* obj_, Func f_): object(obj_), func(f_) {} 
         virtual ~ActiveCallback() {}
         virtual void operator()() { (object->*func)(); } 
      };



   /// Helper functions to make syntax easy, example:
   /// 'send(bind(this, &Active::doFunc));' will run 'doFunc(void)' on 'this'
   template<typename T>
   std::auto_ptr<Callback> bind (T* obj_, void (T::*f_)(void)) {
      Callback* func = new ActiveCallback<T, void>(obj_,f_);
      return std::auto_ptr<Callback> (func);
   }


   template<typename T, typename Param>
   std::auto_ptr<Callback> bind (T* obj_, void(T::*f_)(Param), Param& p_) {
      Callback* func = new ActiveCallback<T, Param> (obj_,f_,p_);
      return std::auto_ptr<Callback>(func);
   }

} // end namespace kjellkod
#endif

