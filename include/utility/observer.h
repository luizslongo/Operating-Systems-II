// EPOS-- Observer Utility Declarations

#ifndef __observer_h
#define	__observer_h

#include <utility/list.h>

__BEGIN_SYS

class Observer;

// Observed (Subject)
class Observed
{ 
protected: 
    Observed() {}

public: 
    virtual ~Observed() {}

    virtual void attach(Observer * o);
    virtual void detach(Observer * o);

    virtual void notify();

private: 
    Simple_List<Observer> _observers;
}; 

// Observer
class Observer
{ 
protected: 
    Observer(): _link(this) {} 

public: 
    virtual ~Observer() {}
    
    virtual void update(Observed * o) = 0;

    friend void Observed::attach(Observer *);
    friend void Observed::detach(Observer *);
    friend void Observed::notify();

private:
    Simple_List<Observer>::Element _link;
};

__END_SYS
 
#endif
