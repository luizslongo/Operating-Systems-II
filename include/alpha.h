#ifndef __alpha_h
#define __alpha_h


#include <utility/list_verified.h>
#include <utility/queue_verified.h>

namespace System
{


class AlphaCriterion 
{
public:    
    enum {
        MAIN = 0,
        NORMAL = 1,            
        IDLE = 2
    };  
    
    static const unsigned int QUEUES = 1;

    
public:
    AlphaCriterion(int p = NORMAL): _priority(p) {}

    operator int() const { return _priority; }
    
    AlphaCriterion & operator-=(const AlphaCriterion& b) 
    {
        _priority = _priority - b._priority;
        return *this;
    }
    
    AlphaCriterion & operator+=(const AlphaCriterion& b) 
    {
        _priority = _priority + b._priority;
        return *this;
    }
    
    bool operator <=(const AlphaCriterion& b) const
    {
        return _priority <= b._priority;
    }
    
    bool operator !=(const AlphaCriterion& b) const
    {
        return _priority != b._priority;
    }
    
    AlphaCriterion operator -(const AlphaCriterion& b) const
    {
        return _priority - b._priority;        
    }
    
    AlphaCriterion operator +(const AlphaCriterion& b) const
    {
        return _priority + b._priority;        
    }

    const int & queue() const { return 0; }
    
    static int current() { return 0; }
    
protected:
    int _priority;
     
};

#if 1 // alpha
class Alpha 
{
    friend class Scheduler<Alpha>;

public:
    typedef AlphaCriterion Criterion;
    enum {
        NORMAL = Criterion::NORMAL,
        MAIN = Criterion::MAIN,
        IDLE = Criterion::IDLE
    };

    /* Alpha Queue */
    typedef Ordered_Queue<Alpha, Criterion, false, Scheduler<Alpha>::Element> Queue; /* Original like Thread.
        In order to work it needs the explicit specialization of Queue_Wrapper of this file. */
   
public:
    Alpha(const Criterion & criterion = NORMAL) : _link(this, criterion) {}
    
protected:
    Queue::Element * link() { return &_link; }
    
protected:
    Queue::Element _link;
    
};
#endif // alpha


#if 0 // AlphaScheduler
class AlphaScheduler : public Scheduler<Alpha> 
{
private:
    typedef Scheduler<Alpha> Base;

public:

    void foo()
    {
        assert(1 == 1);
    }

    Alpha* volatile Scheduler_chosen() 
    {
        // __CPROVER_assume(this);
        return Base::chosen(); 
    }

    void Scheduler_insert_obj(Alpha* obj)
    {
        Base::insert(obj);
    }

    Alpha* Scheduler_remove_obj(Alpha* obj)
    {
        return Base::remove(obj);
    }


    void Scheduler_suspend_obj(Alpha* obj)
    {
        Base::suspend(obj);
    }


    void Scheduler_resume_obj(Alpha* obj)
    {
        Base::resume(obj);
    }

    Alpha* Scheduler_choose()
    {
        return Base::choose();
    }

    Alpha* Scheduler_choose_another()
    {
        return Base::choose_another();
    }

    Alpha* Scheduler_choose_obj(Alpha* obj)
    {
        return Base::choose(obj);
    }

};
#endif // AlphaScheduler


/* Wrapper for non-atomic queues wrapping Ordered_List - Needed because of CBMC 4.1
   not nedded in CBMC 4.2
 */
#if 0
template <>
class Queue_Wrapper<Ordered_List<Alpha, AlphaCriterion, Scheduler<Alpha>::Element>, false> :
	private Ordered_List<Alpha, AlphaCriterion, Scheduler<Alpha>::Element>
{
private:
	typedef typename Ordered_List<Alpha, AlphaCriterion, Scheduler<Alpha>::Element> List;

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

    /* This method does not exist for Ordered_List, so it cannot be wrapped. */
    // Element * volatile & chosen() { return List::chosen(); }

    /* This method does not exist for Ordered_List, so it cannot be wrapped. */
    // Element * choose() { return List::choose(); }

    /* This method does not exist for Ordered_List, so it cannot be wrapped. */
    // Element * choose_another() { return List::choose_another(); }

    /* This method does not exist for Ordered_List, so it cannot be wrapped. */
    // Element * choose(Element * e) { return List::choose(e); }

    /* This method does not exist for Ordered_List, so it cannot be wrapped. */
    // Element * choose(const Object_Type * obj) {	return List::choose(obj); }

};
#endif




}

#endif // __alpha_h
