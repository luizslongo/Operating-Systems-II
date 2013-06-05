// Verified portion of EPOS Scheduler Abstraction Declarations

#ifndef __scheduler_verified_h
#define __scheduler_verified_h

#include <verification_traits.h>

#include <utility/assert.h>

#include <utility/list_verified.h>

namespace System
{

// Scheduling_Queue
template <typename T, unsigned int Q>
class Scheduling_Queue
{
private:
    typedef typename T::Criterion Criterion;
    typedef Scheduling_List<T, Criterion> Queue;

public:
    typedef typename Queue::Element Element;

public:
    Scheduling_Queue()
    {
#if VERIFIED_SCENARIO_ON
    	__CPROVER_assume(Q >= 2); // or 2
#endif
    }

    unsigned int size()
    {
#if VERIFIED_SCENARIO_ON
        invariants();
    	assert(Criterion::current() >= 0);
#endif

        return _ready[Criterion::current()].size(); 
    }

    Element* volatile & chosen()
    {
#if VERIFIED_SCENARIO_ON
        invariants();
        assert(Criterion::current() >= 0);
#endif

        return _ready[Criterion::current()].chosen();
    }

    void insert(Element* e)
    {
    	invariants();

    	_ready[e->rank().queue()].insert(e);
    }

    Element* remove(Element* e)
    {
    	invariants();

        // removing object instead of element forces a search and renders
        // removing inexistent objects harmless
        return _ready[e->rank().queue()].remove(e->object());
    }

    Element* choose()
	{
    	invariants();

        return _ready[Criterion::current()].choose();
    }

    Element* choose_another()
    {
    	invariants();

        return _ready[Criterion::current()].choose_another();
    }

    Element* choose(Element* e)
	{
    	invariants();

        return _ready[e->rank().queue()].choose(e->object());
    }

private:
    Queue _ready[Q];
    
protected:
    bool contains(T* volatile t)
    {
    	invariants();

    	return _ready[Criterion::current()].contains(t);
    }


    bool empty()
	{
    	invariants();

		return size() == 0;
	}

private:
    void invariants()
    {
#if VERIFIED_SCENARIO_ON
    	__CPROVER_assume(this);
#endif
    }
};


// Specialization for single-queue
template <typename T>
class Scheduling_Queue<T, 1>: public Scheduling_List<T, typename T::Criterion>
{
private:
    typedef Scheduling_List<T, typename T::Criterion> Base;

public:
    typedef typename Base::Element Element;

public:
    Element* remove(Element* e)
    {
    	invariants();

    	// removing object instead of element forces a search and renders
    	// removing inexistent objects harmless
        return Base::remove(e->object());
    }

    Element* choose()
	{
    	invariants();

        return Base::choose();
    }

    Element* choose(Element* e)
	{
    	invariants();

		return Base::choose(e->object());
    }

protected:
    bool contains(T* volatile t)
    {
    	invariants();

    	return Base::contains(t);
    }

    bool empty()
	{
    	invariants();

		return Base::empty();
	}


private:
    void invariants()
    {
#if VERIFIED_SCENARIO_ON
    	__CPROVER_assume(this);
#endif
    }

};


// Scheduler
// Objects subject to scheduling by Scheduler must declare a type "Criterion"
// that will be used as the scheduling queue sorting criterion (viz, through 
// operators <, >, and ==) and must also define a method "link" to export the
// list element pointing to the object being handled.
template <typename T>
class Scheduler : public Scheduling_Queue<T, T::Criterion::QUEUES>
{
private:
    typedef Scheduling_Queue<T, T::Criterion::QUEUES> Base;

public:
    typedef typename T::Criterion Criterion;
    typedef Scheduling_List<T, Criterion> Queue;
    typedef typename Queue::Element Element;

public:
    Scheduler()
    {
#if VERIFIED_SCENARIO_ON
    	__CPROVER_assume(T::Criterion::QUEUES >= 1);
#endif
    }

    unsigned int schedulables() 
    {
    	// preconditions
    	// None

    	// class invariants
    	invariants();

    	// body
    	unsigned int s = Base::size();

    	// class invariants
    	invariants();

    	// postconditions
    	// None
        
        return s;
    }


    T* volatile chosen()
    {
    	// preconditions
    	// None

    	// class invariants
#if VERIFIED_SCENARIO_ON
    	__CPROVER_assume(this);
    	invariants();
#endif

    	// body
    	/* NOTE: If Base::chosen can return null then, we have a bug here:
    	 * null->object()
    	 * (trying to dereference a null pointer)
    	 *  */
    	// T* volatile c = const_cast<T* volatile>(Base::chosen()->object());

    	// Fixing the potential bug demands an extra "if"
    	T* volatile c = 0;
    	if (! empty()) {
#if VERIFIED_SCENARIO_ON
            assert(Base::chosen() != 0); // extra
#endif

    		c = const_cast<T* volatile>(Base::chosen()->object());
    	}

#if VERIFIED_SCENARIO_ON
    	// class invariants
		invariants();

    	// postconditions

		if (! empty()) {
			assert(c != 0); // extra
			assert(contains(c));
		}
		else {
			assert(c == 0);
		}
#endif

        return c;
    }


    void insert(T* obj)
    {
#if VERIFICATION_LEVEL_ZERO_ON
        __CPROVER_assume((size() >= 0) && (size() <= LIST_MAX_SIZE));
#endif

        // db<Scheduler>(TRC) << "Scheduler[chosen=" << chosen() << "]::insert(" << obj << ")\n";

    	// preconditions
#if VERIFIED_SCENARIO_ON
        __CPROVER_assume(! contains(obj));
        assert(! contains(obj));
        // assert(contains(obj)); just for fun :P remove this
    	unsigned int size_at_pre = size();
    	__CPROVER_assume((size_at_pre >= 0) && (size_at_pre <= LIST_MAX_SIZE));

        // class invariants
		invariants();
#endif

		// body
        Base::insert(obj->link()); 
        
#if VERIFIED_SCENARIO_ON
        // class invariants
		invariants();

#if VERIFICATION_LEVEL_ZERO_ON
        __CPROVER_assume((size() >= 1) && (size() <= LIST_MAX_SIZE));
#endif

		// postconditions
        assert(contains(obj));
        assert(size() == size_at_pre + 1);
#endif
    }


    T* remove(T* obj)
	{

    	// db<Scheduler>(TRC) << "Scheduler[chosen=" << chosen() << "]::remove(" << obj << ")\n";

    	// preconditions
    	// None
#if VERIFIED_SCENARIO_ON
        __CPROVER_assume(obj);
        assert(obj);

    	bool at_pre_contains_obj = contains(obj);
    	unsigned size_at_pre = Base::size();

    	// class invariants
    	invariants();
#endif

    	// body
    	T* r = Base::remove(obj->link()) ? obj : 0;

#if VERIFIED_SCENARIO_ON
    	// class invariants
    	invariants();

    	// postconditions
    	assert(! contains(obj));
        if (at_pre_contains_obj) {
        	assert(Base::size() == size_at_pre - 1);
        }
        else {
        	assert(r == 0);
        }

#endif

        return r;
    }


    void suspend(T* obj)
    {
    	// db<Scheduler>(TRC) << "Scheduler[chosen=" << chosen() << "]::suspend(" << obj << ")\n";

    	// preconditions
    	// None
#if VERIFIED_SCENARIO_ON
    	bool at_pre_contains_obj = contains(obj);
		unsigned size_at_pre = Base::size();

		// class invariants
		invariants();
#endif

		// body
		Base::remove(obj->link());

#if VERIFIED_SCENARIO_ON
		// class invariants
		invariants();

		// postconditions
		if (at_pre_contains_obj) {
			assert(Base::size() == size_at_pre - 1);
		}

		assert(! contains(obj));
#endif
    }


    void resume(T* obj)
    {
    	// db<Scheduler>(TRC) << "Scheduler[chosen=" << chosen() << "]::resume(" << obj << ")\n";

    	// preconditions
#if VERIFIED_SCENARIO_ON
        __CPROVER_assume(! contains(obj));
        assert(! contains(obj));
        unsigned int size_at_pre = size();
        __CPROVER_assume((size_at_pre >= 0) && (size_at_pre <= LIST_MAX_SIZE));

        // class invariants
        invariants();
#endif

		// body
		Base::insert(obj->link());

#if VERIFIED_SCENARIO_ON
		// class invariants
		invariants();

		// postconditions
        assert(Base::size() == size_at_pre + 1);
        assert(contains(obj));
#endif
    }


    T* choose()
	{
    	// db<Scheduler>(TRC) << "Scheduler[chosen=" << chosen() << "]::choose() => ";

    	// preconditions
    	// None
#if VERIFIED_SCENARIO_ON
    	unsigned int size_at_pre = Base::size();

		// class invariants
		invariants();
#endif

		// body
        T* obj = Base::choose()->object();

#if VERIFIED_SCENARIO_ON
        // class invariants
		invariants();

		// postconditions
		assert(contains(obj));
		assert(Base::size() == size_at_pre);
#endif
		// db<Scheduler>(TRC) << obj << "\n";
	
        return obj;
    }


    T* choose_another()
	{
    	// db<Scheduler>(TRC) << "Scheduler[chosen=" << chosen() << "]::choose_another() => ";

    	// preconditions
    	// None
#if VERIFIED_SCENARIO_ON
    	unsigned int size_at_pre = Base::size();

		// class invariants
		invariants();
#endif

		// body
        T* obj = Base::choose_another()->object();

#if VERIFIED_SCENARIO_ON
        // class invariants
		invariants();

		// postconditions
		assert(contains(obj));
		assert(Base::size() == size_at_pre);
#endif
        // db<Scheduler>(TRC) << obj << "\n";

        return obj;
    }
    

    T* choose(T* obj)
	{
        // db<Scheduler>(TRC) << "Scheduler[chosen=" << chosen() << "]::choose(" << obj;

#if VERIFIED_SCENARIO_ON
        // preconditions
    	bool at_pre_contains_obj = contains(obj);
    	unsigned int size_at_pre = Base::size();

    	// class invariants
		invariants();
#endif

		// body
        if(! Base::choose(obj->link())) {
            obj = 0;
        }

#if VERIFIED_SCENARIO_ON
		// class invariants
		invariants();

		// postconditions
		if (at_pre_contains_obj) {
			assert(contains(obj));
		}
		else {
			assert(obj == 0);
		}
		assert(Base::size() == size_at_pre);
#endif

        // db<Scheduler>(TRC) << obj << "\n";
        
        return obj;
    }


private:
    void invariants()
    {
#if VERIFIED_SCENARIO_ON
    	__CPROVER_assume(this);
    	assert(Base::size() >= 0);
#endif
    }


protected:
    bool empty()
	{
		return Base::empty();
	}


    bool contains(T* volatile t)
    {
        return Base::contains(t);
    }

};

} // namespace System


#endif
