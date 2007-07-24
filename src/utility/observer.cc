// EPOS-- Observer Utility Implementation

#include <utility/debug.h>
#include <utility/observer.h>

__BEGIN_SYS

void Observed::attach(Observer * o)
{
    db<Observed>(TRC) << "Observed::attach(obs=" << o << ")\n";
    _observers.insert(&o->_link);
    db<Observed>(INF) << "teste this=" << this << "\n";
    db<Observed>(INF) << "teste list=" << &_observers << "\n";
    db<Observed>(INF) << "teste head=" << _observers.head() << "\n";

} 

void Observed::detach(Observer * o)
{
    db<Observed>(TRC) << "Observed::detach(obs=" << o << ")\n";
    _observers.remove(&o->_link); 
}

void Observed::notify()
{
    db<Observed>(INF) << "teste this=" << this << "\n";
    db<Observed>(INF) << "teste list=" << &_observers << "\n";
    db<Observed>(INF) << "teste head=" << _observers.head() << "\n";
    for(Simple_List<Observer>::Element * e = _observers.head();
	e; e = e->next()) {
	db<Observed>(TRC) << "Observed::notify(this=" << this
			  << ",obs=" << e->object() << ")\n";
	e->object()->update(this);
    }
} 

__END_SYS
 
