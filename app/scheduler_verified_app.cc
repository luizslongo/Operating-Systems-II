// EPOS Scheduler Verified Abstraction Test Program
// This is just a point (C++ module) to include the EPOS's Scheduler Verified header.


#include <scheduler_verified.h>
#include <alpha.h>

using namespace System;

// Prefer to use the class Alpha defined at alpha.h
#if 0 // A simple Alpha
class Alpha
{
public:
    typedef AlphaCriterion Criterion;
    enum {
        NORMAL = Criterion::NORMAL,
        MAIN = Criterion::MAIN,
        IDLE = Criterion::IDLE
    };

};

#endif // A simple Alpha

#if 0
/* It is necessary to instantiate Ordered_List (i.e. Ordered_List<Alpha>)
 * in order to CBMC find Ordered_List member functions.
 *
 * Related targets on makefile: check_Ordered_List_*
 *
 */
class Ordered_List_Alpha : public Ordered_List<Alpha, Alpha::Criterion>
{

private:
    typedef Ordered_List<Alpha, Alpha::Criterion> Base;

public:
    void ordered_list_insert_element(Element* e)
    {
        Base::insert(e);
    }

    // TODO: other methods

};
#endif


#if 0
/* It is necessary to instantiate List (i.e. List<Alpha>) in order to CBMC
 * find List member functions.
 *
 * Related targets on makefile: check_List_* (all 15).
 *
 */
class AlphaList : public List<Alpha>
{
private:
    typedef List<Alpha> Base;

public:
    Element* list_search(const Object_Type* obj)
    {
        return Base::search(obj);
    }


    void list_insert_element(Element* e)
    {
        Base::insert(e);
    }


    void list_insert_head(Element* e)
    {
        Base::insert_head(e);
    }


    void list_insert_tail(Element* e)
    {
        Base::insert_tail(e);
    }


    Element* list_remove()
	{
    	return Base::remove();
	}


    Element* list_remove_element(Element* e)
	{
    	return Base::remove(e);
	}


    Element* list_remove_head()
	{
    	return Base::remove_head();
	}


    Element* list_remove_tail()
	{
    	return Base::remove_tail();
	}


    Element* list_remove_object(const Object_Type* obj)
	{
    	return Base::remove(obj);
	}

protected:
    void list_insert_element_in_between(Element* e, Element* p,  Element* n)
    {
        Base::insert(e, p, n);
    }


    void list_insert_first(Element* e)
    {
        Base::insert_first(e);
    }


    bool list_contains_element(Element* volatile element)
    {
        return Base::contains_element(element);
    }


    Element* list_remove_last()
	{
    	return Base::remove_last();
	}

    // TODO: other methods


};
#endif


#if 0
/* It is necessary to instantiate Scheduling_List (i.e. Scheduling_List<Alpha>)
 * in order to CBMC find Ordered_List member functions.
 *
 * Related targets on makefile: check_Scheduling_List_*
 *
 */
class Scheduling_List_Alpha : public Scheduling_List<Alpha, Alpha::Criterion>
{

private:
    typedef Scheduling_List<Alpha, Alpha::Criterion> Base;

public:
    void scheduling_List_insert_element(Element* e)
    {
        Base::insert(e);
    }

    // TODO: other methods

};
#endif


#if 0 // For now, disabling AlphaScheduler
void check_alpha_scheduler()
{
    assert(0 == 0);

    AlphaScheduler* scheduler = new AlphaScheduler();
    assert(0 == 1);

    Alpha* alpha = new Alpha();
    assert(0 == 2);

    scheduler->chosen();
    assert(0 == 3);

    scheduler->insert(alpha);
    assert(0 == 4);

    scheduler->chosen();
    assert(0 == 5);

    delete scheduler;
    assert(0 == 6);
}
#endif // For now, disabling AlphaScheduler


class SchedulerCheck
{
public:
    void checkConstructionDestruction()
    {
        setUp();
        tearDown();
    }


    void checkSchedulables()
    {
        setUp();
        __scheduler->schedulables();
        tearDown();
    }


    void checkChosen()
    {
        setUp();
        __scheduler->chosen();
        tearDown();
    }


    void checkInsert()
    {
        Alpha* a = new Alpha();

        setUp();
        __scheduler->insert(a);
        tearDown();

        delete a;
    }


    void checkRemove()
    {
        Alpha* a = new Alpha();
        Alpha* b = new Alpha();

        setUp();
        __scheduler->remove(0);

        __scheduler->remove(b);

        __scheduler->insert(a);
        __scheduler->remove(a);

        tearDown();

        delete a;
        delete b;
    }


    void checkSuspend()
    {
        Alpha* a = new Alpha();
        Alpha* b = new Alpha();

        setUp();
        __scheduler->suspend(0);

        __scheduler->suspend(b);

        __scheduler->insert(a);
        __scheduler->suspend(a);

        tearDown();

        delete a;
        delete b;
    }


    void checkResume()
    {
        Alpha* a = new Alpha();
        Alpha* b = new Alpha();

        setUp();
        __scheduler->resume(0);

        __scheduler->resume(b);

        __scheduler->insert(a);
        __scheduler->suspend(a);
        __scheduler->resume(a);

        tearDown();

        delete a;
        delete b;
    }


    void checkChoose()
    {
        Alpha* a = new Alpha();

        setUp();
        __scheduler->choose();

        __scheduler->insert(a);
        __scheduler->choose();
        tearDown();

        delete a;
    }


    void checkChooseAnother()
    {
        Alpha* a = new Alpha();

        setUp();
        __scheduler->choose_another();

        __scheduler->insert(a);
        __scheduler->choose_another();
        tearDown();

        delete a;
    }


    void checkChooseObj()
    {
        Alpha* a = new Alpha();
        Alpha* b = new Alpha();
        setUp();

        __scheduler->choose(0);

        __scheduler->choose(b);

        __scheduler->insert(a);
        __scheduler->choose(a);

        tearDown();
        delete a;
        delete b;
    }


    void check()
    {
        checkConstructionDestruction();

        // 9 public methods of scheduler
        checkSchedulables();
        checkChosen();
        checkInsert();
        checkRemove();
        checkSuspend();
        checkResume();
        checkChoose();
        checkChooseAnother();
        checkChooseObj();
    }


protected:
    void setUp()
    {
        __scheduler = new Scheduler<Alpha>();
    }


    void tearDown()
    {
        delete __scheduler;
    }


private:
    Scheduler<Alpha>* __scheduler;

};


int main()
{
    assert(0 == 0); // trace XXX

    SchedulerCheck* c = new SchedulerCheck();
    
    c->check();
    
    assert(0 == 1); // trace XXX

    delete c;

    return 0;
}
