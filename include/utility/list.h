// EPOS-- List Utility Declarations

#ifndef __list_h
#define	__list_h

#include <system/config.h>

__BEGIN_SYS

// List Elements
class List_Elements
{
public:
    // Element for vector
    template <typename T>
    class Pointer
    {
    public:
	typedef T Object_Type;
	typedef Pointer Element;

    public:
	Pointer(const T * o): _object(o) {}
    
	T * object() const { return const_cast<T *>(_object); }

    private:
	const T * _object;
    };

    // Element for hash table
    template <typename T>
    class Ranked
    {
    public:
	typedef T Object_Type;
	typedef Ranked Element;

    public:
	Ranked(const T * o, int r = 0): _object(o), _rank(r) {}

	T * object() const { return const_cast<T *>(_object); }

	int rank() const { return _rank; }
	int key() const { return _rank; }
	void rank(int r) { _rank = r; }
	int promote(int n = 1) { _rank -= n; return _rank; }
	int demote(int n = 1) { _rank += n; return _rank; }

    private:
	const T * _object;
 	int _rank;
   };

    // Element for singly-linked list
    template <typename T>
    class Singly_Linked
    {
    public:
	typedef T Object_Type;
	typedef Singly_Linked Element;

    public:
	Singly_Linked(const T * o): _object(o), _next(0) {}
    
	T * object() const { return const_cast<T *>(_object); }

	Element * next() const { return _next; }
	void next(Element * e) { _next = e; }
    
    private:
	const T * _object;
	Element * _next;
    };

    // Element for singly-linked, ordered list and hash
    template <typename T>
    class Singly_Linked_Ordered
    {
    public:
	typedef T Object_Type;
	typedef Singly_Linked_Ordered Element;

    public:
	Singly_Linked_Ordered(const T * o, int r = 0): _object(o), _rank(r),
						       _next(0) {}
    
	T * object() const { return const_cast<T *>(_object); }

	Element * next() const { return _next; }
	void next(Element * e) { _next = e; }

	int rank() const { return _rank; }
	int key() const { return _rank; }
	void rank(int r) { _rank = r; }
	int promote(int n = 1) { _rank -= n; return _rank; }
	int demote(int n = 1) { _rank += n; return _rank; }
    
    private:
	const T * _object;
	int _rank;
	Element * _next;
    };

    // Element for singly-linked, grouping list
    template <typename T>
    class Singly_Linked_Grouping
    {
    public:
	typedef T Object_Type;
	typedef Singly_Linked_Grouping Element;

    public:
	Singly_Linked_Grouping(const T * o, int s):
	    _object(o), _size(s), _next(0) {}

	T * object() const { return const_cast<T *>(_object); }

	Element * next() const { return _next; }
	void next(Element * e) { _next = e; }

	unsigned int size() const { return _size; }
	void size(unsigned int l) { _size = l; }
	void shrink(unsigned int n) { _size -= n; }
	void expand(unsigned int n) { _size += n; }
    
    private:
	const T * _object;
	unsigned int _size;
	Element * _next;
    };

    // Element for doubly-linked list
    template <typename T>
    class Doubly_Linked
    {
    public:
	typedef T Object_Type;
	typedef Doubly_Linked Element;

    public:
	Doubly_Linked(const T * o): _object(o), _prev(0), _next(0) {}
    
	T * object() const { return const_cast<T *>(_object); }

	Element * prev() const { return _prev; }
	Element * next() const { return _next; }
	void prev(Element * e) { _prev = e; }
	void next(Element * e) { _next = e; }
    
    private:
	const T * _object;
	Element * _prev;
	Element * _next;
    };

    // Element for ordered list
    template <typename T>
    class Doubly_Linked_Ordered
    {
    public:
	typedef T Object_Type;
	typedef Doubly_Linked_Ordered Element;

    public:
	Doubly_Linked_Ordered(const T * o, int r = 0): _object(o), _rank(r),
						       _prev(0), _next(0) {}
    
	T * object() const { return const_cast<T *>(_object); }

	Element * prev() const { return _prev; }
	Element * next() const { return _next; }
	void prev(Element * e) { _prev = e; }
	void next(Element * e) { _next = e; }

	int rank() const { return _rank; }
	void rank(int r) { _rank = r; }
	int promote(int n = 1) { _rank -= n; return _rank; }
	int demote(int n = 1) { _rank += n; return _rank; }
    
    private:
	const T * _object;
	int _rank;
	Element * _prev;
	Element * _next;
    };
    
    // Element for grouping list
    template <typename T>
    class Doubly_Linked_Grouping
    {
    public:
	typedef T Object_Type;
	typedef Doubly_Linked_Grouping Element;

    public:
	Doubly_Linked_Grouping(const T * o, int s): _object(o), _size(s),
						    _prev(0), _next(0) {}

	T * object() const { return const_cast<T *>(_object); }

	Element * prev() const { return _prev; }
	Element * next() const { return _next; }
	void prev(Element * e) { _prev = e; }
	void next(Element * e) { _next = e; }

	unsigned int size() const { return _size; }
	void size(unsigned int l) { _size = l; }
	void shrink(unsigned int n) { _size -= n; }
	void expand(unsigned int n) { _size += n; }
    
    private:
	const T * _object;
	unsigned int _size;
	Element * _prev;
	Element * _next;
    };
};


// Singly-Linked List
template <typename T, typename El = List_Elements::Singly_Linked<T> >
class Simple_List
{
public:
    typedef T Object_Type;
    typedef El Element;

public:
    Simple_List(): _size(0), _head(0), _tail(0) {}

    bool empty() const { return (_size == 0); }
    unsigned int size() const { return _size; }

    Element * head() { return _head; }
    Element * tail() { return _tail; }

    void insert(Element * e) { insert_tail(e); }
    void insert_head(Element * e) {
	if(empty())
	    insert_first(e);
	else {
	    e->next(_head);
	    _head = e;
	    _size++;
	}
    }
    void insert_tail(Element * e) {
	if(empty())
	    insert_first(e);
	else {
	    _tail->next(e);
	    e->next(0);
	    _tail = e;
	    _size++;
	}
    }
    
    Element * remove() { return remove_head(); }
    Element * remove(Element * e) {
	if(last())
	    remove_last();
	else if(e == _head)
	    remove_head();
	    else {
		Element * p = _head;
		for(; p && p->next() && (p->next() != e); p = p->next());
		if(p)
		    p->next(e->next());
		_size--;
	    }
	return e;
    }
    Element * remove_head() {
	if(empty())
	    return 0;
	if(last())
	    return remove_last();
	Element * e = _head;
	_head = _head->next();
	_size--;
	return e;
    }
    Element * remove_tail() {
	if(_tail)
		return remove(_tail);
    }
    Element * remove(const Object_Type * obj) {
	Element * e = search(obj);
	if(e)
	    return remove(e);
	else
	    return 0;
    }
    
    Element * search(const Object_Type * obj) {
	Element * e = _head;
	    for(; e && (e->object() != obj); e = e->next());
	    return e;
    }
    
protected:
    bool last() const { return (_size == 1); }
    void insert(Element * e, Element * p,  Element * n) {
	p->next(e);
	e->next(n);
	_size++;
    }
    void insert_first(Element * e) {
	e->next(0);
	_head = e;
	_tail = e;
	_size++;
    }
    Element * remove_last() {
	Element * e = _head;
	_head = 0;
	_tail = 0;
	_size--;
	return e;
    }

private:
    unsigned int _size;
    Element * _head;
    Element * _tail;
};


// Singly-Linked, Ordered List
template <typename T, typename El = List_Elements::Singly_Linked_Ordered<T>,
	  bool relative = false>
class Simple_Ordered_List: public Simple_List<T, El>
{
public:
    typedef T Object_Type;
    typedef El Element;

public:
    using Simple_List<T, El>::empty;
    using Simple_List<T, El>::head;
    using Simple_List<T, El>::remove_head;
    
    void insert(Element * e) {
	if(empty())
	    insert_first(e);
	else {
	    Element * next, * prev;
	    for(next = head(), prev = 0;
		(next->rank() <= e->rank()) && next->next();
		prev = next, next = next->next())
		if(relative)
		    e->rank(e->rank() - next->rank());
	    if(next->rank() <= e->rank()) {
		if(relative)
		    e->rank(e->rank() - next->rank());
		insert_tail(e);
	    } else if(!prev) {
		if(relative)
		    next->rank(next->rank() - e->rank());
		insert_head(e);
	    } else {
		if(relative)
		    next->rank(next->rank() - e->rank());
		Simple_List<T, El>::insert(e, prev, next);
	    }
	}
    }
    
    Element * remove() { return remove_head(); }
    Element * remove(Element * e) {
	Simple_List<T, El>::remove(e);
	if(relative && e->next())
	    e->next()->rank(e->next()->rank() + e->rank());
	return e;
    }
    Element * remove(const Object_Type * obj) {
	Element * e = search(obj);
	if(e)
	    return remove(e);
	return 0;
    }
    
    Element * search_rank(int rank) {
	Element * e = head();
	for(; e && (e->rank() != rank); e = e->next());
	return e;
    }
    Element * remove_rank(int rank) {
	Element * e = search_rank(rank);
	if(e)
	    return remove(e);
	return 0;
    }
};


// Singly-Linked, Relative Ordered List
template <typename T, typename El = List_Elements::Singly_Linked_Ordered<T> >
class Simple_Relative_List: public Simple_Ordered_List<T, El, true> {};


// Singly-Linked, Grouping List
template <typename T, typename El = List_Elements::Singly_Linked_Grouping<T> >
class Simple_Grouping_List: public Simple_List<T, El>
{
public:
    typedef T Object_Type;
    typedef El Element;

public:
    Simple_Grouping_List(): _grouped_size(0) {}

    using Simple_List<T, El>::head;

    unsigned int grouped_size() const { return _grouped_size; }

    Element * search_size(unsigned int s) {
	Element * e = head();
	for(; e && (e->size() < s); e = e->next());
	return e;
    }
    
    Element * search_left(const Object_Type * obj) {
	Element * e = head();
	for(; e && (e->object() + e->size() != obj); e = e->next());
	return e;
    }
    
    void insert_merging(Element * e, Element ** m1, Element ** m2) {
	_grouped_size += e->size();
	*m1 = *m2 = 0;
	Element * r = search(e->object() + e->size());
	Element * l = search_left(e->object());
	if(!r && !l)
	    insert_tail(e);
	else {
	    if(r) {
		e->size(e->size() + r->size());
		remove(r);
		*m1 = r;
	    }
	    if(l) {
		l->size(l->size() + e->size());
		*m2 = e;
	    }
	}
    }
    
    Element * search_decrementing(unsigned int s) {
	Element * e = search_size(s);
	if(e) {
	    e->shrink(s);
	    _grouped_size -= s;
	    if(!e->size())
		remove(e);
	}
	return e;
    }
    
private:
    unsigned int _grouped_size;
};


// Doubly-Linked List
template <typename T, typename El = List_Elements::Doubly_Linked<T> >
class List
{
public:
    typedef T Object_Type;
    typedef El Element;

public:
    List(): _size(0), _head(0), _tail(0) {}

    bool empty() const { return (_size == 0); }
    unsigned int size() const { return _size; }
    
    Element * head() { return _head; }
    Element * tail() { return _tail; }
    
    void insert(Element * e) { insert_tail(e); }
    void insert_head(Element * e) {
	if(empty())
	    insert_first(e);
	else {
	    e->prev(0);
	    e->next(_head);
	    _head->prev(e);
	    _head = e;
	    _size++;
	}
    }
    void insert_tail(Element * e) {
	if(empty())
	    insert_first(e);
	else {
	    _tail->next(e);
	    e->prev(_tail);
	    e->next(0);
	    _tail = e;
	    _size++;
	}
    }
    
    Element * remove() { return remove_head(); }
    Element * remove(Element * e) {
	if(last())
	    remove_last();
	else if(!e->prev())
	    remove_head();
	else if(!e->next())
	    remove_tail();
	else {
	    e->prev()->next(e->next());
	    e->next()->prev(e->prev());
	    _size--;
	}
	return e;
    }
    Element * remove_head() {
	if(empty())
	    return 0;
	if(last())
	    return remove_last();
	Element * e = _head;
	_head = _head->next();
	_head->prev(0);
	_size--;
	return e;
    }
    Element * remove_tail() {
	if(empty())
	    return 0;
	if(last())
	    return remove_last();
	Element * e = _tail;
	_tail = _tail->prev();
	_tail->next(0);
	_size--;
	return e;
    }
    Element * remove(const Object_Type * obj) {
	Element * e = search(obj);
	if(e)
	    return remove(e);
	else
	    return 0;
    }
    
    Element * search(const Object_Type * obj) {
	Element * e = _head;
	for(; e && (e->object() != obj); e = e->next());
	return e;
    }
    
protected:
    bool last() const { return (_size == 1); }
    void insert(Element * e, Element * p,  Element * n) {
	p->next(e);
	n->prev(e);
	e->prev(p);
	e->next(n);
	_size++;
    }
    void insert_first(Element * e) {
	e->prev(0);
	e->next(0);
	_head = e;
	_tail = e;
	_size++;
    }
    Element * remove_last() {
	Element * e = _head;
	_head = 0;
	_tail = 0;
	    _size--;
	    return e;
    }
    
private:
    unsigned int _size;
    Element * _head;
    Element * _tail;
};


// Doubly-Linked, Ordered List
template <typename T, typename El = List_Elements::Doubly_Linked_Ordered<T>,
	  bool relative = false>
class Ordered_List: public List<T, El>
{
public:
    typedef T Object_Type;
    typedef El Element;

public:
    using List<T, El>::empty;
    using List<T, El>::head;
    using List<T, El>::remove_head;
    
    void insert(Element * e) {
	if(empty())
	    insert_first(e);
	else {
	    Element * next;
	    for(next = head();
		(next->rank() <= e->rank()) && next->next();
		next = next->next())
		if(relative)
		    e->rank(e->rank() - next->rank());
	    if(next->rank() <= e->rank()) {
		if(relative)
		    e->rank(e->rank() - next->rank());
		insert_tail(e);
	    } else if(!next->prev()) {
		if(relative)
		    next->rank(next->rank() - e->rank());
		insert_head(e);
		} else {
		if(relative)
		    next->rank(next->rank() - e->rank());
		List<T, El>::insert(e, next->prev(), next);
	    }
	}
    }
    
    Element * remove() { return remove_head(); }
    Element * remove(Element * e) {
	List<T, El>::remove(e);
	if(relative && e->next())
	    e->next()->rank(e->next()->rank() + e->rank());
	return e;
    }
    Element * remove(const Object_Type * obj) {
	Element * e = search(obj);
	if(e)
	    return remove(e);
	else
	    return 0;
    }
    
    Element * search_rank(int rank) {
	Element * e = head();
	for(; e && (e->rank() != rank); e = e->next());
	return e;
    }
    Element * remove_rank(int rank) {
	Element * e = search_rank(rank);
	if(e)
	    return remove(e);
	else
	    return 0;
    }
};


// Doubly-Linked, Relative Ordered List
template <typename T, typename El = List_Elements::Doubly_Linked_Ordered<T> >
class Relative_List: public Ordered_List<T, El, true> {};


// Doubly-Linked, Grouping List
template <typename T, typename El = List_Elements::Doubly_Linked_Grouping<T> >
class Grouping_List: public List<T, El>
{
public:
    typedef T Object_Type;
    typedef El Element;

public:
    using List<T, El>::head;

    Grouping_List(): _grouped_size(0) {}

    unsigned int grouped_size() const { return _grouped_size; }
    
    Element * search_size(unsigned int s) {
	Element * e = head();
	for(; e && (e->size() < s); e = e->next());
	return e;
    }
    
    Element * search_left(const Object_Type * obj) {
	Element * e = head();
	for(; e && (e->object() + e->size() != obj); e = e->next());
	return e;
    }
    
    void insert_merging(Element * e, Element ** m1, Element ** m2) {
	_grouped_size += e->size();
	*m1 = *m2 = 0;
	Element * r = search(e->object() + e->size());
	Element * l = search_left(e->object());
	if(!r && !l)
	    insert_tail(e);
	else {
	    if(r) {
		e->size(e->size() + r->size());
		remove(r);
		*m1 = r;
	    }
	    if(l) {
		l->size(l->size() + e->size());
		*m2 = e;
	    }
	}
    }
    
    Element * search_decrementing(unsigned int s) {
	Element * e = search_size(s);
	if(e) {
	    e->shrink(s);
	    _grouped_size -= s;
	    if(!e->size())
		remove(e);
	}
	return e;
    }
    
private:
    unsigned int _grouped_size;
};

__END_SYS
 
#endif
