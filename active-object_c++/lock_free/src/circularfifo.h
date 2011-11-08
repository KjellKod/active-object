/* CircularFifo.h
* Not any company's property but Public-Domain
* Do with source-code as you will. No requirement to keep this
* header if need to use it/change it/ or do whatever with it
*
* Note that there is No guarantee that this code will work
* and I take no responsibility for this code and any problems you
* might get if using it. The code is highly platform dependent!
*
* Code & platform dependent issues with it was originally
* published at http://www.kjellkod.cc/threadsafecircularqueue
* 2009-11-02
* @author Kjell Hedström, hedstrom@kjellkod.cc */

#ifndef CIRCULARFIFO_H_
#define CIRCULARFIFO_H_

namespace kjellkod {

  /** Circular Fifo (a.k.a. Circular Buffer)
  * Thread safe for one reader, and one writer */
  template<typename Element, unsigned int Size>
  class CircularFifo {
    private:
      static const int Capacity = Size+1;
      volatile unsigned int tail; 
      Element array[Capacity];
      volatile unsigned int head; 
      unsigned int increment(unsigned int idx_) const;

    public:

      CircularFifo() : tail(0), head(0) {}
      virtual ~CircularFifo() {}

      bool push(Element& item_);
      bool pop(Element& item_);
      bool isEmpty() const;
      bool isFull() const;
      
      int nbrOfItems() const; // Test purpose only     
    };




  /** Producer: Adds item to the circular queue.
  * If queue is full at 'push' operation no update/overwrite
  * will happen, it is up to the caller to handle this case
  * \param item_ copy by reference the input item
  * \return whether operation was successful or not */
  template<typename Element, unsigned int Size>
  bool CircularFifo<Element, Size>::push(Element& item_) {
    unsigned int nextTail = increment(tail);
    if (nextTail != head) {
        array[tail] = item_;
        tail = nextTail;
        return true;
      }
    return false; // full queue
  }

  /** Consumer: Removes and returns item from the queue
  * If queue is empty at 'pop' operation no retrieve will happen
  * It is up to the caller to handle this case
  * \param item_ return by reference the wanted item
  * \return whether operation was successful or not */
  template<typename Element, unsigned int Size>
  bool CircularFifo<Element, Size>::pop(Element& item_) {
    if (head == tail)
      return false;  // empty queue

    item_ = array[head];
    head = increment(head);
    return true;
  }

  /** Consumer: Useful for testing and Consumer check of status
    * Remember that the 'empty' status can change quickly
    * as the Procuder adds more items.
    * \return true if circular buffer is empty */
  template<typename Element, unsigned int Size>
  bool CircularFifo<Element, Size>::isEmpty() const {
      return (head == tail);
    }


  /** Producer: Useful for testing and Producer check of status
    * Remember that the 'full' status can change quickly
    * as the Consumer catches up.
    * \return true if circular buffer is full.  */
  template<typename Element, unsigned int Size>
  bool CircularFifo<Element, Size>::isFull() const {
      unsigned int tailCheck = (tail+1) % Capacity;
      return (tailCheck == head);
    }

  /** TEST: Test purpose only: checking current active items in the queue
  *   this result is not to be trusted if using threads for input/output
  *   since the content can change whenever 
  *   \return snapshot of number of items int the queue */  
  template<typename Element, unsigned int Size>
  int CircularFifo<Element, Size>::nbrOfItems() const{    
      if(isEmpty())
        return 0;
      else if(isFull())
        return Size;
      else
        return (tail - head + ((tail < head) ? Capacity : 0));         
    }


  /** Internal: Increment helper function for index of the circular queue
  * index is incremented or wrapped
  *  \param idx_ the index to the incremented/wrapped
  *  \return new value for the index */
  template<typename Element, unsigned int Size>
  unsigned int CircularFifo<Element, Size>::increment(unsigned int idx_) const {
      // increment or wrap: index++ then   if(index == array.lenght) set index = 0;
      idx_ = (idx_+1) % Capacity;
      return idx_;
    }

} // end namespace kjellkod

#endif /* CIRCULARFIFO_H_ */
