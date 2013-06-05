// Verified portion of EPOS Queue Utility Declarations

#ifndef __queue_verified_h
#define	__queue_verified_h

#include "list_verified.h"

namespace System //__BEGIN_SYS
{

// Wrapper for non-atomic queues  
template <typename List, bool atomic>
class Queue_Wrapper: private List
{
public:
    typedef typename List::Object_Type Object_Type;
    typedef typename List::Element Element;

public:
    void lock() { }
    void unlock() { }

    bool empty() { return List::empty(); }
    unsigned int size() { return List::size(); }

    Element * head() { return List::head(); }
    Element * tail() { return List::tail(); }

    void insert(Element * e) { List::insert(e); }

    Element * remove() { return List::remove(); }
    Element * remove(Element * e) { return List::remove(e); }
    Element * remove(const Object_Type * obj) { return List::remove(obj); }

    Element * search(const Object_Type * obj) {	return List::search(obj); }


    // This method does not exist for Ordered_List, so it cannot be wrapped.
    // Element * volatile & chosen() { return List::chosen(); }


    // This method does not exist for Ordered_List, so it cannot be wrapped.
    //Element* choose() { return List::choose(); }


    // This method does not exist for Ordered_List, so it cannot be wrapped.
    // Element* choose_another() { return List::choose_another(); }


    // This method does not exist for Ordered_List, so it cannot be wrapped.
    // Element* choose(Element* e) { return List::choose(e); }


    // This method does not exist for Ordered_List, so it cannot be wrapped.
    // Element* choose(const Object_Type* obj) {	return List::choose(obj); }
};

// Wrapper for atomic queues  
template <typename List>
class Queue_Wrapper<List, true>: private List
{
private:
    static const bool smp = Traits<Thread>::smp;

public:
    typedef typename List::Object_Type Object_Type;
    typedef typename List::Element Element;

public:
    void lock() { _lock.acquire(); }
    void unlock() { _lock.release(); }

    bool empty() {
	enter();
	bool tmp = List::empty();
	leave();
	return tmp;
    }

    unsigned int size() {
	enter(); 
	unsigned int tmp = List::size();
	leave();
	return tmp;
    }

    Element * head() { 
	enter(); 
	Element * tmp = List::head();
	leave();
	return tmp;
    }

    Element * tail() { 
	enter(); 
	Element * tmp = List::tail();
	leave();
	return tmp;
    }

    void insert(Element * e) { 
	enter(); 
	List::insert(e);
	leave();
    }

    Element * remove() { 
	enter(); 
	Element * tmp = List::remove();
	leave();
	return tmp;
    }

    Element * remove(const Object_Type * obj) {
	enter(); 
	Element * tmp = List::remove(obj); 
	leave();
	return tmp;
    }

    Element * search(const Object_Type * obj) {
	enter(); 
	Element * tmp = List::search(obj);
	leave();
	return tmp;
    }

    // This method does not exist for Ordered_List, so it cannot be wrapped.
    /*
    Element* volatile & chosen() {
        enter();
        Element * volatile & tmp = List::chosen();
        leave();

        return tmp;
    }
    */

    // This method does not exist for Ordered_List, so it cannot be wrapped.
    /*
    Element* choose() {
        enter();
        Element* tmp = List::choose();
        leave();
        return tmp;
    }
    */


    // This method does not exist for Ordered_List, so it cannot be wrapped.
    /*
    Element* choose_another()
    {
        enter();
        Element* tmp = List::choose_another();
        leave();
        return tmp;
    }
    */


    // This method does not exist for Ordered_List, so it cannot be wrapped.
    /*
    Element* choose(Element* e)
    {
        enter();
        Element* tmp = List::choose(e);
        leave();
        return tmp;
    }
    */


    // This method does not exist for Ordered_List, so it cannot be wrapped.
    /*
    Element* choose(const Object_Type* obj)
    {
        enter();
        Element* tmp = List::choose(obj);
        leave();
        return tmp;
    }
    */

private:
    void enter() {
	CPU::int_disable();
	if(smp) _lock.acquire(); 
    }

    void leave() {
	if(smp) _lock.release();
	CPU::int_disable();
    }

private:
    Spin _lock;
};


// Queue
template <typename T,
	  bool atomic = false,
	  typename El = List_Elements::Doubly_Linked<T> >
class Queue: public Queue_Wrapper<List<T, El>, atomic> {};


// Ordered Queue
template <typename T, 
	  typename R = List_Element_Rank, 
	  bool atomic = false,
	  typename El = List_Elements::Doubly_Linked_Ordered<T, R> >
class Ordered_Queue:
    public Queue_Wrapper<Ordered_List<T, R, El>, atomic> {};


// Relatively-Ordered Queue
template <typename T, 
	  typename R = List_Element_Rank, 
	  bool atomic = false,
	  typename El = List_Elements::Doubly_Linked_Ordered<T, R> >
class Relative_Queue:
    public Queue_Wrapper<Relative_List<T, R, El>, atomic> {};

} // __END_SYS
 
#endif

