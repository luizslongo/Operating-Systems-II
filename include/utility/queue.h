// EPOS-- Queue Utility Declarations

// Queue<T, false, false> is a traditional queue, with insertions at the tail
// and removals either from the head or from specific objects 

// Queue<T, true, false> is an ordered queue, i.e. objects are inserted
// in-order based on the integral value of "element.rank". Note that "rank"
// implies an order, but does not necessarily need to be "the absolute order"
// in the queue; it could, for instance, be a priority information or a 
// time-out specification. Insertions must first tag "element" with "rank".
// Removals are just like in the traditional queue. Elements of both Queues
// may be exchanged.
// Example: insert(B,7);insert(C,9);insert(A,4)
// +---+		+---+	+---+	+---+
// |obj|		| A |-->| B |-->| C |
// + - +       head -->	+ - +	+ - +	+ - + <-- tail
// |ord|		| 4 |<--| 7 |<--| 9 |
// +---+ 		+---+	+---+	+---+

// Queue<T, true, true> is an ordered queue, i.e. objects are inserted
// in-order based on the integral value of "element.rank" just like above.
// But differently from that, a Relative Queue handles "rank" as relative 
// offsets. This is very useful for alarm queues. Elements of Relative Queue
// cannot be exchanged with elements of the other queues described earlier.
// Example: insert(B,7);insert(C,9);insert(A,4)
// +---+		+---+	+---+	+---+
// |obj|		| A |-->| B |-->| C |
// + - +       head -->	+ - +	+ - +	+ - + <-- tail
// |ord|		| 4 |<--| 3 |<--| 2 |
// +---+ 		+---+	+---+	+---+

#ifndef __queue_h
#define	__queue_h

#include "list.h"
#include "spin.h"

__BEGIN_SYS

template <typename T, bool ordered, bool relative, bool atomic>
class Queue_Common: private List<T, ordered, relative>
{
private:
    typedef List<T, ordered, relative> List;

public:
    typedef typename List::Element Element;

public:
    bool empty() const { return List::empty(); }
    unsigned int size() const { return List::size(); }

    Element * head() { return List::head(); }
    Element * tail() { return List::tail(); }

    void insert(Element * e) { List::insert(e); }

    Element * remove() { return List::remove(); }
    Element * remove(const T * obj) { return List::remove(obj); }

    Element * search(const T * obj) { return List::search(obj); }
};

template <typename T, bool ordered, bool relative>
class Queue_Common<T, ordered, relative, true>
    : private List<T, ordered, relative>
{
private:
    typedef List<T, ordered, relative> List;

public:
    typedef typename List::Element Element;

public:
    bool empty() {
	_lock.acquire(); 
	bool tmp = List::empty();
	_lock.release();
	return tmp;
    }
    unsigned int size() {
	_lock.acquire(); 
	unsigned int tmp = List::size();
	_lock.release();
	return tmp;
    }

    Element * head() { 
	_lock.acquire(); 
	Element * tmp = List::head();
	_lock.release();
	return tmp;
    }
    Element * tail() { 
	_lock.acquire(); 
	Element * tmp = List::tail();
	_lock.release();
	return tmp;
    }

    void insert(Element * e) { 
	_lock.acquire(); 
	List::insert(e);
	_lock.release();
    }

    Element * remove() { 
	_lock.acquire(); 
	Element * tmp = List::remove();
	_lock.release();
	return tmp;
    }
    Element * remove(const T * obj) {
	_lock.acquire(); 
	Element * tmp = List::remove(obj); 
	_lock.release();
	return tmp;
    }

    Element * search(const T * obj) {
	_lock.acquire(); 
	Element * tmp = List::search(obj);
	_lock.release();
	return tmp;
    }

private:
    Spin _lock;
};

template <typename T, bool atomic = true>
class Queue: public Queue_Common<T, false, false, atomic>
{};

template <typename T, bool atomic = true>
class Ordered_Queue: public Queue_Common<T, true, false, atomic>
{};

template <typename T, bool atomic = true>
class Relative_Queue: public Queue_Common<T, true, true, atomic>
{};

__END_SYS
 
#endif
