// EPOS-- List Utility Declarations

#ifndef __list_h
#define	__list_h

#include <system/config.h>

__BEGIN_SYS

class List_Common
{
public:
    // List elements template
    template <typename T, bool doubly, bool ordered, bool grouped>
    class Element;

    // Element for singly linked list
    template <typename T>
    class Element<T, false, false, false>
    {
    public:
	typedef T Object_Type;

    public:
	Element(const T * o): _object(o), _next(0) {}
    
	T * object() const { return const_cast<T *>(_object); }
	Element * next() const { return _next; }
	void next(Element * e) { _next = e; }
    
    private:
	const T * _object;
	Element * _next;
    };

    // Element for singly linked, grouping list
    template <typename T>
    class Element<T, false, false, true>
    {
    public:
	typedef T Object_Type;

    public:
	Element(const T * o, int s): _object(o), _size(s), _next(0) {}

	T * object() const { return const_cast<T *>(_object); }
	Element * next() const { return _next; }
	unsigned int size() const { return _size; }
	void next(Element * e) { _next = e; }
	void size(unsigned int l) { _size = l; }
    
	void shrink(unsigned int n) { _size -= n; }
	void expand(unsigned int n) { _size += n; }
    
    private:
	const T * _object;
	unsigned int _size;
	Element * _next;
    };

    // Element for doubly linked list
    template <typename T>
    class Element<T, true, false, false>
    {
    public:
	typedef T Object_Type;

    public:
	Element(const T * o): _object(o), _prev(0), _next(0) {}
    
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
    class Element<T, true, true, false>
    {
    public:
	typedef T Object_Type;

    public:
	Element(const T * o, int r = 0): _object(o), _rank(r),
					 _prev(0), _next(0) {}
    
	T * object() const { return const_cast<T *>(_object); }
	Element * prev() const { return _prev; }
	Element * next() const { return _next; }
	int rank() const { return _rank; }
	void prev(Element * e) { _prev = e; }
	void next(Element * e) { _next = e; }
	void rank(int r) { _rank = r; }
    
    private:
	const T * _object;
	int _rank;
	Element * _prev;
	Element * _next;
    };
    
    // Element for grouping list
    template <typename T>
    class Element<T, true, false, true>
    {
    public:
	typedef T Object_Type;

    public:
	Element(const T * o, int s): _object(o), _size(s),
				     _prev(0), _next(0) {}

	T * object() const { return const_cast<T *>(_object); }
	Element * prev() const { return _prev; }
	Element * next() const { return _next; }
	unsigned int size() const { return _size; }
	void prev(Element * e) { _prev = e; }
	void next(Element * e) { _next = e; }
	void size(unsigned int l) { _size = l; }
    
	void shrink(unsigned int n) { _size -= n; }
	void expand(unsigned int n) { _size += n; }
    
    private:
	const T * _object;
	unsigned int _size;
	Element * _prev;
	Element * _next;
    };

    // Element for ordered grouping list
    template <typename T>
    class Element<T, true, true, true>
    {
    public:
	typedef T Object_Type;

    public:
	Element(const T * o, int r, int s): _object(o), _rank(r), _size(s),
					    _prev(0), _next(0) {}

	T * object() const { return const_cast<T *>(_object); }
	Element * prev() const { return _prev; }
	Element * next() const { return _next; }
	int rank() const { return _rank; }
	unsigned int size() const { return _size; }
	void prev(Element * e) { _prev = e; }
	void next(Element * e) { _next = e; }
	void rank(int o) { _rank = o; }
	void size(unsigned int l) { _size = l; }
    
	void shrink(unsigned int n) { _size -= n; }
	void expand(unsigned int n) { _size += n; }
    
    private:
	const T * _object;
	int _rank;
	unsigned int _size;
	Element * _prev;
	Element * _next;
    };


    // List algorithms template
    template <typename El, 
	      bool doubly, bool ordering, bool relative, bool grouping>
    class Algorithm;

    // Algorithm for singly linked list
    template <typename El>
    class Algorithm<El, false, false, false, false>
    {
    private:
	typedef El Element;
	typedef typename Element::Object_Type Object_Type;

    public:
	Algorithm(): _size(0), _head(0), _tail(0) {}

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

    // Algorithm for singly linked, grouping list
    template <typename El>
    class Algorithm<El, false, false, false, true>:
	public Algorithm<El, false, false, false, false>
    {
    private:
	typedef El Element;
	typedef typename Element::Object_Type Object_Type;

    public:
	Algorithm(): _grouped_size(0) {}

	using Algorithm<El, false, false, false, false>::head;

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

    // Algorithm for doubly linked list
    template <typename El>
    class Algorithm<El, true, false, false, false>
    {
    private:
	typedef El Element;
	typedef typename Element::Object_Type Object_Type;

    public:
	Algorithm(): _size(0), _head(0), _tail(0) {}

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

    // Algorithm for (relative) ordered list
    template <typename El, bool relative>
    class Algorithm<El, true, true, relative, false>:
	public Algorithm<El, true, false, false, false>
    {
    private:
	typedef El Element;
	typedef typename Element::Object_Type Object_Type;

    public:
	using Algorithm<El, true, false, false, false>::empty;
	using Algorithm<El, true, false, false, false>::head;
	using Algorithm<El, true, false, false, false>::remove_head;

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
		    Algorithm<Element, true, false, false,
			false>::insert(e, next->prev(), next);
		}
	    }
	}

 	Element * remove() { return remove_head(); }
	Element * remove(Element * e) {
	    Algorithm<Element, true, false, false, false>::remove(e);
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
    };

    // Algorithm for grouping list
    template <typename El>
    class Algorithm<El, true, false, false, true>:
	public Algorithm<El, true, false, false, false>
    {
    private:
	typedef El Element;
	typedef typename Element::Object_Type Object_Type;

    public:
	using Algorithm<El, true, false, false, false>::head;

	Algorithm(): _grouped_size(0) {}

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
};

template <typename T, bool grouping = false>
class Simple_List: public List_Common::Algorithm<
    List_Common::Element<T, false, false, grouping>,
    false, 
    false,
    false, 
    grouping>
{
public:
    typedef typename List_Common::Element<T, false, false, grouping> Element;
};

template <typename T>
class Simple_Grouping_List: public Simple_List<T, true> {};

template <typename T,
	  bool ordered = false, bool relative = false, bool grouping = false>
class List: public List_Common::Algorithm<
    List_Common::Element<T, true, ordered, grouping>,
    true,
    ordered,
    relative,
    grouping>
{
public:
    typedef typename List_Common::Element<T, true, ordered, grouping> Element;
};

template <typename T, bool relative = false>
class Ordered_List: public List<T, true, relative, false> {};

template <typename T>
class Relative_List: public Ordered_List<T, true> {};

template <typename T>
class Grouping_List: public List<T, false, false, true> {};

__END_SYS
 
#endif
