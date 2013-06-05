// Verified portion of EPOS List Utility Declarations


#ifndef __list_verified_h
#define __list_verified_h

#include <verification_traits.h>

namespace System
{

// List Element Rank (for ordered lists)
class List_Element_Rank
{
public:
    List_Element_Rank(int r = 0): _rank(r) {}

    operator int() const { return _rank; }

#if VERIFICATION_LEVEL_ZERO_ON
    // Required by CBMC
    List_Element_Rank & operator-=(const List_Element_Rank& b)
    {
        _rank = _rank - b._rank;
        return *this;
    }

    List_Element_Rank & operator+=(const List_Element_Rank& b)
    {
        _rank = _rank + b._rank;
       return *this;
    }

    bool operator <=(const List_Element_Rank& b) const
    {
       return _rank <= b._rank;
    }

    List_Element_Rank operator -(const List_Element_Rank& b) const
    {
        return _rank - b._rank;
    }

    List_Element_Rank operator +(const List_Element_Rank& b) const
    {
        return _rank + b._rank;
    }

    bool operator !=(const List_Element_Rank& b) const
    {
        return _rank != b._rank;
    }

#endif

protected:
    int _rank;
};


// List Elements
namespace List_Elements
{
    typedef List_Element_Rank Rank;

    // Vector Element
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

    // Hash Table Element
    template <typename T, typename R = Rank>
    class Ranked
    {
    public:
        typedef T Object_Type;
        typedef R Rank_Type;
        typedef Ranked Element;

    public:
        Ranked(const T * o, const R & r = 0): _object(o), _rank(r) {}

        T * object() const { return const_cast<T *>(_object); }

        const R & rank() const { return _rank; }
        const R & key() const { return _rank; }
        void rank(const R & r) { _rank = r; }
        int promote(const R & n = 1) { _rank -= n; return _rank; }
        int demote(const R & n = 1) { _rank += n; return _rank; }

    private:
        const T * _object;
         R _rank;
   };

    // Simple List Element
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

    // Simple Ordered List Element
    // Hash Table's Synonym List Element
    template <typename T, typename R = Rank>
    class Singly_Linked_Ordered
    {
    public:
        typedef T Object_Type;
        typedef Rank Rank_Type;
        typedef Singly_Linked_Ordered Element;

    public:
        Singly_Linked_Ordered(const T * o, const R & r = 0):
            _object(o), _rank(r), _next(0) {}
    
        T * object() const { return const_cast<T *>(_object); }

        Element * next() const { return _next; }
        void next(Element * e) { _next = e; }

        const R & rank() const { return _rank; }
        const R & key() const { return _rank; }
        void rank(const R & r) { _rank = r; }
        int promote(const R & n = 1) { _rank -= n; return _rank; }
        int demote(const R & n = 1) { _rank += n; return _rank; }

    private:
        const T * _object;
         R _rank;
        Element * _next;
    };

    // Simple Grouping List Element
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

    // List Element
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

    // Ordered List Element
    template <typename T, typename R = Rank>
    class Doubly_Linked_Ordered
    {
    public:
        typedef T Object_Type;
        typedef Rank Rank_Type;
        typedef Doubly_Linked_Ordered Element;

    public:
        Doubly_Linked_Ordered(const T * o,  const R & r = 0):
            _object(o), _rank(r), _prev(0), _next(0) {}
    
        T * object() const { return const_cast<T *>(_object); }

        Element * prev() const { return _prev; }
        Element * next() const { return _next; }
        void prev(Element * e) { _prev = e; }
        void next(Element * e) { _next = e; }

        const R & rank() const { return _rank; }
        void rank(const R & r) { _rank = r; }
        int promote(const R & n = 1) { _rank -= n; return _rank; }
        int demote(const R & n = 1) { _rank += n; return _rank; }

    private:
        const T * _object;
         R _rank;
        Element * _prev;
        Element * _next;
    };
    
    // Scheduling List Element
    template <typename T, typename R = Rank>
    class Doubly_Linked_Scheduling
    {
    public:
        typedef T Object_Type;
        typedef Rank Rank_Type;
        typedef Doubly_Linked_Scheduling Element;

    public:
        Doubly_Linked_Scheduling(const T * o,  const R & r = 0):
            _object(o), _rank(r), _prev(0), _next(0) {}
    
        T * object() const { return const_cast<T *>(_object); }

        Element * prev() const { return _prev; }
        Element * next() const { return _next; }
        void prev(Element * e) { _prev = e; }
        void next(Element * e) { _next = e; }

        const R & rank() const { return _rank; }
        void rank(const R & r) { _rank = r; }
        int promote(const R & n = 1) { _rank -= n; return _rank; }
        int demote(const R & n = 1) { _rank += n; return _rank; }

    private:
        const T * _object;
        R _rank;
        Element * _prev;
        Element * _next;
    };


    // Grouping List Element
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


// List Iterators
namespace List_Iterators
{
    // Forward Iterator (for singly linked lists)
    template<typename El>
    class Forward
    {
    private:
        typedef Forward<El> Iterator;

    public:
        typedef El Element;

    public:
        Forward(): _current(0) {}
        Forward(Element * e): _current(e) {}

        operator Element *() const { return _current; }

        Element & operator*() const { return *_current; }
        Element * operator->() const { return _current; }

        Iterator & operator++() {
            _current = _current->next(); return *this;
        }
        Iterator operator++(int) {
            Iterator tmp = *this; ++*this; return tmp;
        }

        bool operator==(const Iterator & i) const {
            return _current == i._current;
        }
        bool operator!=(const Iterator & i) const {
            return _current != i._current;
        }

    private:
        Element * _current;
    };

    // Bidireacional Iterator (for doubly linked lists)
    template<typename El>
    class Bidirecional
    {
    private:
        typedef Bidirecional<El> Iterator;

    public:
        typedef El Element;

    public:
        Bidirecional(): _current(0) {}
        Bidirecional(Element * e): _current(e) {}

        operator Element *() const { return _current; }

        Element & operator*() const { return *_current; }
        Element * operator->() const { return _current; }

        Iterator & operator++() {
            _current = _current->next(); return *this;
        }
        Iterator operator++(int) {
            Iterator tmp = *this; ++*this; return tmp;
        }

        Iterator & operator--() {
            _current = _current->prev(); return *this;
        }
        Iterator operator--(int) {
            Iterator tmp = *this; --*this; return tmp;
        }
        
        bool operator==(const Iterator & i) const {
            return _current == i._current;
        }
        bool operator!=(const Iterator & i) const {
            return _current != i._current;
        }

    private:
        Element * _current;
    };
}


#if VERIFICATION_LEVEL_ZERO_ON
unsigned int const LIST_MAX_SIZE = 3;
#endif


// Singly-Linked List
template <typename T, 
          typename El = List_Elements::Singly_Linked<T> >
class Simple_List
{
public:
    typedef T Object_Type;
    typedef El Element;
    typedef List_Iterators::Forward<El> Iterator;

public:
    Simple_List(): _size(0), _head(0), _tail(0) {}

    bool empty() const { return (_size == 0); }
    unsigned int size() const { return _size; }

    Element * head() { return _head; }
    Element * tail() { return _tail; }

    Iterator begin() { return Iterator(_head); }
    Iterator end() { return Iterator(_tail ? _tail->next() : 0); }

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
        if(e == _tail)
            _tail = p;
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
        else
            return 0;
    }

    Element * remove(const Object_Type * obj) {
        Element * e = search(obj);
        if(e)
            return remove(e);
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
template <typename T, 
          typename R = List_Element_Rank, 
          typename El = List_Elements::Singly_Linked_Ordered<T, R>,
          bool relative = false>
class Simple_Ordered_List: public Simple_List<T, El>
{
private:
    typedef Simple_List<T, El> Base;

public:
    typedef T Object_Type;
    typedef R Rank_Type;
    typedef El Element;
    typedef List_Iterators::Forward<El> Iterator;

public:
    using Base::empty;
    using Base::size;
    using Base::head;
    using Base::begin;
    using Base::end;
    using Base::remove_head;
    
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
                Base::insert(e, prev, next);
            }
        }
    }
    
    Element * remove() { return remove_head(); }

    Element * remove(Element * e) {
        Base::remove(e);
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
template <typename T, 
          typename R = List_Element_Rank,
          typename El = List_Elements::Singly_Linked_Ordered<T, R> >
class Simple_Relative_List: public Simple_Ordered_List<T, R, El, true> {};


// Singly-Linked, Grouping List
template <typename T,
          typename El = List_Elements::Singly_Linked_Grouping<T> >
class Simple_Grouping_List: public Simple_List<T, El>
{
private:
    typedef Simple_List<T, El> Base;

public:
    typedef T Object_Type;
    typedef El Element;
    typedef List_Iterators::Forward<El> Iterator;

public:
    Simple_Grouping_List(): _grouped_size(0) {}

    using Base::empty;
    using Base::size;
    using Base::head;
    using Base::tail;
    using Base::begin;
    using Base::end;

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
        if(r) {
            e->size(e->size() + r->size());
            remove(r);
            *m1 = r;
        }
        if(l) {
            l->size(l->size() + e->size());
            *m2 = e;
        } else
            insert_tail(e);
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
template <typename T,
          typename El = List_Elements::Doubly_Linked<T> >
class List
{
private:
    void invariants() const
    {
#if VERIFIED_SCENARIO_ON
    	__CPROVER_assume(this);
#endif
    }

public:
    typedef T Object_Type;
    typedef El Element;
    typedef List_Iterators::Bidirecional<El> Iterator;

public:
    List(): _size(0), _head(0), _tail(0) {}

    bool empty() const
	{
    	//invariants();
    	return (_size == 0);
	}

    unsigned int size() const
	{
    	invariants();
    	return _size;
    }
    
    Element * head()
	{
    	invariants();
    	return _head;
	}

    Element * tail()
	{
    	invariants();
    	return _tail;
	}
    
    Iterator begin()
    {
    	invariants();
    	return Iterator(_head);
    }

    Iterator end()
    {
    	invariants();
    	return Iterator(_tail->next());
    }

    void insert(Element* e)
    {
#if VERIFIED_SCENARIO_ON
        // class invariants
        invariants();
        // preconditions
        __CPROVER_assume(e && (! contains_element(e)));
        assert(e && (! contains_element(e)));
#endif

    	insert_tail(e);

#if VERIFIED_SCENARIO_ON
        // class invariants
        invariants();
        // postconditions
        __CPROVER_assume(e && (_size <= LIST_MAX_SIZE));
        assert(e && (contains_element(e)));
#endif

    }

    void insert_head(Element* e)
    {
#if VERIFIED_SCENARIO_ON
        // class invariants
        invariants();
        // preconditions
        __CPROVER_assume(e && (! contains_element(e)));
        assert(e && (! contains_element(e)));
#endif

        /*
        db<Lists>(TRC) << "List::insert_head(e=" << e 
                      << ") => {p=" << (e ? e->prev() : (void *) -1)
                      << ",o=" << (e ? e->object() : (void *) -1)
                      << ",n=" << (e ? e->next() : (void *) -1)
                      << "}\n";
         */ 

        print_head();
        print_tail();

        if(empty())
            insert_first(e);
        else {
            e->prev(0);
            e->next(_head);
            _head->prev(e);
            _head = e;
            _size++;
        }

        print_head();
        print_tail();

#if VERIFIED_SCENARIO_ON
        // class invariants
        invariants();
        // postconditions
        __CPROVER_assume(e && (_size <= LIST_MAX_SIZE));
        assert(e && (contains_element(e)));
#endif

    }

    void insert_tail(Element* e)
    {
#if VERIFIED_SCENARIO_ON
        // class invariants
        invariants();
        // preconditions
        __CPROVER_assume(e && (_size <= LIST_MAX_SIZE) && (! contains_element(e)));
        assert(e && (! contains_element(e)));
#endif

        /*
        db<Lists>(TRC) << "List::insert_tail(e=" << e 
                      << ") => {p=" << (e ? e->prev() : (void *) -1)
                      << ",o=" << (e ? e->object() : (void *) -1)
                      << ",n=" << (e ? e->next() : (void *) -1)
                      << "}\n";
                      */ 

        print_head();
        print_tail();

        if(empty()) {
            insert_first(e);
        }
        else {
            _tail->next(e);
            e->prev(_tail);
            e->next(0);
            _tail = e;
            _size++;
        }

        print_head();
        print_tail();

#if VERIFIED_SCENARIO_ON
        // class invariants
        invariants();
        // postconditions
        __CPROVER_assume(e && (_size <= LIST_MAX_SIZE));
        assert(e && (contains_element(e)));
#endif

    }
    
    Element* remove()
	{
    	invariants();
    	return remove_head();
	}

    Element* remove(Element* e)
    {
    	invariants();
        /*
        db<Lists>(TRC) << "List::remove(e=" << e 
                      << ") => {p=" << (e ? e->prev() : (void *) -1)
                      << ",o=" << (e ? e->object() : (void *) -1)
                      << ",n=" << (e ? e->next() : (void *) -1)
                      << "}\n";
                      */ 

        print_head();
        print_tail();

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

        print_head();
        print_tail();

        return e;
    }


    /*! Removes list head if there is one (i.e. list not empty).
     *  Returns null otherwise.
     * */
    Element* remove_head()
	{
#if VERIFIED_SCENARIO_ON
    	// class invariants
    	invariants();

    	// preconditions
    	// None
    	bool empty_at_pre = empty();
#endif
        /* db<Lists>(TRC) << "List::remove_head()\n";
         */

    	// body
        print_head();
        print_tail();

        if(empty()) {
            return 0;
        }
        if(last()) {
            return remove_last();
        }
        Element* e = _head;
        _head = _head->next();
        _head->prev(0);
        _size--;

        print_head();
        print_tail();

#if VERIFIED_SCENARIO_ON
    	// class invariants
    	invariants();

    	// postconditions
    	if (empty_at_pre) {
    		assert(e == 0);
    	}
    	else {
    		assert(e);
    		assert(e && (! contains_element(e)));
    	}
#endif

        return e;
    }

    Element* remove_tail()
	{
    	invariants();
        /*
        db<Lists>(TRC) << "List::remove_tail()\n";
        */ 

        print_head();
        print_tail();

        if(empty())
            return 0;
        if(last())
            return remove_last();
        Element * e = _tail;
        _tail = _tail->prev();
        _tail->next(0);
        _size--;

        print_head();
        print_tail();

        return e;
    }

    Element* remove(const Object_Type* obj)
	{
    	invariants();
        Element* e = search(obj);
        if(e) {
            return remove(e);
        }
        return 0;
    }
    

    /*! Returns the element if "obj" is in the list. Otherwise returns null.
     * */
    Element* search(const Object_Type* obj)
	{
    	invariants();

#if VERIFICATION_LEVEL_ZERO_ON
    	__CPROVER_assume(obj && (_size <= LIST_MAX_SIZE)); // In order to bound the loop
#endif
        Element* e = _head;
        // loop modified because of CBMC (added variable "i" to control the loop bounds)
        int i;
        for(i = 0; e && (e->object() != obj) && (i < _size); e = e->next(), i++);

        return e;
    }
    
protected:
    bool last() const
    {
    	invariants();
    	return (_size == 1);
    }

    void insert(Element* e, Element* p,  Element* n)
    {
#if VERIFIED_SCENARIO_ON
        // class invariants
        invariants();
        // preconditions
        __CPROVER_assume(e && p && n && (! contains_element(e)));
        assert(e && (! contains_element(e)));
#endif

        /*
        db<Lists>(TRC) << "List::insert(e=" << e << ",p=" << p << ",n=" << n
                      << ") => {p=" << (e ? e->prev() : (void *) -1)
                      << ",o=" << (e ? e->object() : (void *) -1)
                      << ",n=" << (e ? e->next() : (void *) -1)
                      << "},{p=" << (p ? p->prev() : (void *) -1)
                      << ",o=" << (p ? p->object() : (void *) -1)
                      << ",n=" << (p ? p->next() : (void *) -1)
                      << "},{p=" << (n ? n->prev() : (void *) -1)
                      << ",o=" << (n ? n->object() : (void *) -1)
                      << ",n=" << (n ? n->next() : (void *) -1)
                      << "}\n";
                      */

        print_head();
        print_tail();

        p->next(e);
        n->prev(e);
        e->prev(p);
        e->next(n);
        _size++;

        print_head();
        print_tail();

#if VERIFIED_SCENARIO_ON
        // class invariants
        invariants();
        // postconditions
        __CPROVER_assume(e && (_size <= LIST_MAX_SIZE));
        assert(e && (contains_element(e)));
#endif

    }

    void insert_first(Element* e)
    {
#if VERIFIED_SCENARIO_ON
        // class invariants
        invariants();
        // preconditions
        __CPROVER_assume(e && (_size <= LIST_MAX_SIZE) && (! contains_element(e)));
        assert(e && (! contains_element(e)));
#endif

        /*
        db<Lists>(TRC) << "List::insert_first(e=" << e 
                      << ") => {p=" << (e ? e->prev() : (void *) -1)
                      << ",o=" << (e ? e->object() : (void *) -1)
                      << ",n=" << (e ? e->next() : (void *) -1)
                      << "}\n";
                      */ 

        print_head();
        print_tail();

        e->prev(0);
        e->next(0);
        _head = e;
        _tail = e;
        _size++;

        print_head();
        print_tail();

#if VERIFIED_SCENARIO_ON
        // class invariants
        invariants();
        // postconditions
        //__CPROVER_assume(e && (_size <= LIST_MAX_SIZE) && (contains_element(e)));
        __CPROVER_assume(e && (_size <= LIST_MAX_SIZE));
        assert(e && (contains_element(e)));
#endif
    }

    Element* remove_last()
	{
#if VERIFIED_SCENARIO_ON
    	invariants();

    	// preconditions
    	__CPROVER_assume(_head);
    	__CPROVER_assume(_size == 1);
    	assert(_head);
#endif
        /*
        db<Lists>(TRC) << "List::remove_last()\n";
        */ 

        print_head();
        print_tail();

        Element* e = _head;
        _head = 0;
        _tail = 0;
        _size--;

        print_head();
        print_tail();

#if VERIFIED_SCENARIO_ON
        // class invariants
        invariants();
        // postconditions
        assert(_size == 0);
        assert(e);
        assert(! contains_element(e));
        assert(e && (! contains_element(e)));

#endif

        return e;
    }

    void print_head() {
        /*
        db<Lists>(INF) << "List[" << this << "]::head=" << head() 
                      << " => {p=" << (head() ? head()->prev() : (void *) -1)
                      << ",o=" << (head() ? head()->object() : (void *) -1)
                      << ",n=" << (head() ? head()->next() : (void *) -1)
                      << "}\n";
                      */ 
    }

    void print_tail() {
        /*
        db<Lists>(INF) << "List[" << this << "]::tail=" << tail() 
                       << " => {p=" << (tail() ? tail()->prev() : (void *) -1)
                       << ",o=" << (tail() ? tail()->object() : (void *) -1)
                       << ",n=" << (tail() ? tail()->next() : (void *) -1)
                       << "}\n";
                       */ 
    }

    bool contains_element(Element* volatile element)
	{
#if VERIFICATION_LEVEL_ZERO_ON
    	__CPROVER_assume((this) && (_size >= 0) && (_size <= LIST_MAX_SIZE));
    	if (_size >= 1) {
    		__CPROVER_assume(_head);
    	}
    	else {
    		assert(_size == 0);
    		assert(_head == 0);
    	}

    	invariants();
#endif
    	Element* e;
    	int i;

    	for(e = _head, i = 0; e && (i < _size); e = e->next(), i++) {
    		if (e == element) {
    			return true;
    		}

    	}
    	return false;
	}

private:
    unsigned int _size;
    Element * _head;
    Element * _tail;

};


// Doubly-Linked, Ordered List
template <typename T,
          typename R = List_Element_Rank,
          typename El = List_Elements::Doubly_Linked_Ordered<T, R>,
          bool relative = false>
class Ordered_List: public List<T, El>
{
private:
    typedef List<T, El> Base;

public:
    typedef T Object_Type;
    typedef R Rank_Type;
    typedef El Element;
    typedef List_Iterators::Bidirecional<El> Iterator;

public:
    using Base::empty;
    using Base::size;
    using Base::head;
    using Base::tail;
    using Base::begin;
    using Base::end;

    void insert(Element* e)
    {
#if VERIFIED_SCENARIO_ON
        // class invariants
        invariants();
        // preconditions
        __CPROVER_assume(e && (! contains_element(e)));
        assert(e && (! contains_element(e)));
#endif

        /*
        db<Lists>(TRC) << "Ordered_List::insert(e=" << e 
                      << ") => {p=" << (e ? e->prev() : (void *) -1)
                      << ",o=" << (e ? e->object() : (void *) -1)
                      << ",n=" << (e ? e->next() : (void *) -1)
                      << "}\n";
                      */

        // body
        if(empty()) {
            insert_first(e);
        }
        else {
            Element* next;
#if VERIFICATION_LEVEL_ZERO_ON
            __CPROVER_assume(size() <= LIST_MAX_SIZE); // In order to bound the loop
#endif
            // loop modified because of CBMC (added variable "i" to control the loop bounds)
            int i;
            for(next = head(), i = 0;
                (next->rank() <= e->rank()) && next->next() && (i < size());
                next = next->next(), i++)
            {
                if(relative) {
                    e->rank(e->rank() - next->rank());
                }
            }
            if(next->rank() <= e->rank()) {
                if(relative) {
                    e->rank(e->rank() - next->rank());
                }
                insert_tail(e);
            } else if(!next->prev()) {
                if(relative) {
                    next->rank(next->rank() - e->rank());
                }
                insert_head(e);
            } else {
                if(relative) {
                    next->rank(next->rank() - e->rank());
                }
                Base::insert(e, next->prev(), next);
            }
        }

#if VERIFIED_SCENARIO_ON
        // class invariants
        invariants();
        // postconditions
        __CPROVER_assume(e && (_size <= LIST_MAX_SIZE));
        assert(e && (contains_element(e)));
#endif
    }
    

    Element* remove()
    {
    	invariants();
        /* db<Lists>(TRC) << "Ordered_List::remove()\n";
         */ 

        return Base::remove_head();
    }


    Element* remove(Element* e)
    {
    	invariants();
        /*
        db<Lists>(TRC) << "Ordered_List::remove(e=" << e 
                      << ") => {p=" << (e ? e->prev() : (void *) -1)
                      << ",o=" << (e ? e->object() : (void *) -1)
                      << ",n=" << (e ? e->next() : (void *) -1)
                      << "}\n";
                      */ 

    	// body
        Base::remove(e);
        if(relative && e->next()) {
            e->next()->rank(e->next()->rank() + e->rank());
        }

        return e;
    }


    Element * remove(const Object_Type* obj)
    {
    	invariants();
        /* db<Lists>(TRC) << "Ordered_List::remove(o=" << obj << ")\n";
         */ 

        Element * e = search(obj);
        if(e)
            return remove(e);
        return 0;
    }
    
    Element* search_rank(int rank)
    {
    	invariants();
        Element * e = head();
        for(; e && (e->rank() != rank); e = e->next());
        return e;
    }


    Element* remove_rank(int rank)
    {
    	invariants();
        /*
        db<Lists>(TRC) << "Ordered_List::remove_rank(r=" << rank << ")\n";
        */ 

        Element* e = search_rank(rank);
        if(e) {
            return remove(e);
        }

        return 0;
    }

protected:
    bool contains_element(Element* volatile e)
	{
    	invariants();
		return Base::contains_element(e);
	}


    Element * remove_last()
	{
    	invariants();
    	Base::remove_last();
	}


private:
    void invariants()
    {
#if VERIFIED_SCENARIO_ON
    	__CPROVER_assume(this);
#endif
    }

};


// Doubly-Linked, Relative Ordered List
template <typename T,
          typename R = List_Element_Rank,
          typename El = List_Elements::Doubly_Linked_Ordered<T, R> >
class Relative_List: public Ordered_List<T, R, El, true> {};


// Doubly-Linked, Scheduling List
template <typename T,
          typename R = List_Element_Rank, 
          typename El = List_Elements::Doubly_Linked_Scheduling<T, R> >
class Scheduling_List: private Ordered_List<T, R, El>
{
private:
    typedef Ordered_List<T, R, El> Base;

public:
    typedef T Object_Type;
    typedef R Rank_Type;
    typedef El Element;
    typedef typename Base::Iterator Iterator;

public:
    Scheduling_List(): _chosen(0) {}

    using Base::empty;
    using Base::size;
    using Base::head;
    using Base::tail;
    using Base::begin;
    using Base::end;

    Element* volatile & chosen()
    {
    	invariants();
		// preconditions
    	// None
    	//
    	/* size() > 0 is semantically equivalent to ! empty() but, for some
    	 * mysterious reason CBMC does not think so.
    	 * */
    	// assert(size() > 0);
    	// assert(! empty());

    	// class invariants
    	// None (by now)

    	// body
    	// at the end

    	// class invariants
		// None (by now)

    	// postconditions
        #if VERIFIED_SCENARIO_ON /* temporarily disabled */
		if (! empty())
		{
            assert(! empty()); // extra
            assert(_chosen != 0); // extra
			assert(contains_element(_chosen));
		}
		else
		{
            assert(empty()); // extra
			assert(_chosen == 0);
		}
        #endif


    	return _chosen;
    }

    void insert(Element* e)
    {
        /*
        db<Lists>(TRC) << "Scheduling_List::insert(e=" << e 
                      << ") => {p=" << (e ? e->prev() : (void *) -1)
                      << ",o=" << (e ? e->object() : (void *) -1)
                      << ",n=" << (e ? e->next() : (void *) -1)
                      << "}\n";
                      */
#if VERIFIED_SCENARIO_ON
        // class invariants
        invariants();
    	// preconditions
        //__CPROVER_assume(e && (_size <= LIST_MAX_SIZE) && (! contains_element(e)));
        __CPROVER_assume(e && (! contains_element(e)));
    	assert(e &&	(! contains_element(e)));
#endif

		// body
    	Base::insert(e);
    	if(! _chosen) {
    		_chosen = head();
    	}

    	// postconditions
    	// assert(head());
#if VERIFIED_SCENARIO_ON
        // class invariants
        invariants();
        // postconditions
        __CPROVER_assume(e && (_size <= LIST_MAX_SIZE));
        assert(e && (contains_element(e)));
#endif
    }

    Element* remove(Element* e)
    {
    	invariants();
        /*
        db<Lists>(TRC) << "Scheduling_List::remove(e=" << e 
                      << ") => {p=" << (e ? e->prev() : (void *) -1)
                      << ",o=" << (e ? e->object() : (void *) -1)
                      << ",n=" << (e ? e->next() : (void *) -1)
                      << "}\n";
                      */ 
#if VERIFIED_SCENARIO_ON
    	// precondition
    	assert(e);
#endif

		// body
        Base::remove(e);
        if(e == _chosen) {
            _chosen = head();
        }

        /* Not necessary if remove_last is called correctly */
		if (empty()) {
			_chosen = 0;
		}

        return e;
    }

    Element* remove(const Object_Type* obj)
    {
    	invariants();
        Element* e = search(obj);
        if(e) {
            return remove(e);
        }
        else {
            return 0;
        }
    }

    Element* choose()
    {
    	invariants();
        /*
        db<Lists>(TRC) << "Scheduling_List::choose()\n";
        */ 

        if(!empty()) {
            reorder();
            _chosen = head();
        }

        return _chosen;
    }

    Element* choose_another()
    {
    	invariants();
        /*
        db<Lists>(TRC) << "Scheduling_List::choose_another()\n";
        */ 

        if(size() > 1) {
            Element * tmp = _chosen;
            Base::remove(tmp);
            _chosen = Base::head();
            Base::insert(tmp);
        }

        return _chosen;
    }

    Element* choose(Element* e)
    {
    	invariants();
        /*
        db<Lists>(TRC) << "Scheduling_List::choose(e=" << e 
                      << ") => {p=" << (e ? e->prev() : (void *) -1)
                      << ",o=" << (e ? e->object() : (void *) -1)
                      << ",n=" << (e ? e->next() : (void *) -1)
                      << "}\n";
                      */ 

        if(_chosen)
            reorder();
        _chosen = e;

        return _chosen;
    }

    Element* choose(const Object_Type * obj)
    {
    	invariants();
        Element * e = search(obj);
        if(e)
            return choose(e);
        else
            return 0;
    }

protected:
    bool contains(T* volatile t)
    {
    	invariants();
    	__CPROVER_assume(t);

    	return (Base::search(t) != 0);
    }


    Element * remove_last()
	{
    	invariants();
    	Base::remove_last();
    	_chosen = 0;
	}


private:
    void reorder()
    {
    	invariants();
        // Rank might have changed, so remove and insert to reorder
        Base::remove(_chosen);
        Base::insert(_chosen);
    }
    
    bool contains_element(Element* volatile e)
    {
    	invariants();
    	return Base::contains_element(e);
    }

private:
    Element * volatile _chosen;


private:
    void invariants()
    {
#if VERIFIED_SCENARIO_ON
    	__CPROVER_assume(this);
#endif
    }
};


// Doubly-Linked, Grouping List
template <typename T, 
          typename El = List_Elements::Doubly_Linked_Grouping<T> >
class Grouping_List: public List<T, El>
{
private:
    typedef List<T, El> Base;

public:
    typedef T Object_Type;
    typedef El Element;
    typedef List_Iterators::Bidirecional<El> Iterator;

public:
    Grouping_List(): _grouped_size(0) {}

    using Base::empty;
    using Base::size;
    using Base::head;
    using Base::tail;
    using Base::begin;
    using Base::end;

    unsigned int grouped_size() const { return _grouped_size; }
    
    Element * search_size(unsigned int s) {
        Element * e = head();
        for(; e && (e->size() < sizeof(Element) + s) && (e->size() != s); e = e->next());
        return e;
    }
    
    Element * search_left(const Object_Type * obj) {
        Element * e = head();
        for(; e && (e->object() + e->size() != obj); e = e->next());
        return e;
    }
    
    void insert_merging(Element * e, Element ** m1, Element ** m2) {
        /*
        db<Lists>(TRC) << "Grouping_List::insert_merging(e=" << e << ")    \n";
        */
         
        _grouped_size += e->size();
        *m1 = *m2 = 0;
        Element * r = search(e->object() + e->size());
        Element * l = search_left(e->object());
        if(!l) {
            insert_tail(e);
        }
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
    
    Element * search_decrementing(unsigned int s) {
        /*
        db<Lists>(TRC) << "Grouping_List::search_decrementing(s=" 
            << s << ")\n";
            */ 

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

} // namespace System
 
#endif
