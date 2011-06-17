// EPOS AIX4LITE Device Common Package Declarations

#ifndef __aix4lite_device_h
#define __aix4lite_device_h

#include <system/config.h>
#include <utility/list.h>

__BEGIN_SYS

class AIX4LITE_Device 
{
private:
    typedef Simple_List<AIX4LITE_Device> List;

public:
    AIX4LITE_Device(const Type_Id & type, unsigned int unit, void * dev,
              unsigned int interrupt = ~0U)
        : _type(type), _unit(unit), _object(dev), _interrupt(interrupt),
          _busy(false), _link(this) { 
        _devices.insert(&_link);
    }

    ~AIX4LITE_Device() { _devices.remove(&_link); }

    void * object() { return _object; }

    static void * seize(const Type_Id & type, unsigned int unit) {
        AIX4LITE_Device * dev = get(type, unit);
        if(!dev) {
            db<AIX4LITE>(WRN) << "AIX4LITE_Device::seize: device not found\n";
            return 0;
        }
        if(dev->_busy) {
            db<AIX4LITE>(WRN) << "AIX4LITE_Device::seize: device busy\n";
            return 0;
        }
        dev->_busy = true;

        db<AIX4LITE>(TRC) << "AIX4LITE_Device::seize(type=" << type << ",unit=" << unit 
                    << ") => " << dev << "\n";

        return dev->_object;
    }

    static void release(const Type_Id & type, unsigned int unit) {
        AIX4LITE_Device * dev = get(type, unit);
        if(!dev)
            db<AIX4LITE>(WRN) << "AIX4LITE_Device::release: device not found\n";
        dev->_busy = false; 
    }

    static AIX4LITE_Device * get(const Type_Id & type, unsigned int unit) {
        List::Element * e = _devices.head();
        for(; e && ((e->object()->_type != type) ||
                    (e->object()->_unit != unit)); e = e->next());
        if(!e)
            return 0;
        return e->object();
    }

    static AIX4LITE_Device * get(unsigned int interrupt) {
        List::Element * e = _devices.head();
        for(; e && (e->object()->_interrupt != interrupt); e = e->next());
        if(!e)
            return 0;
        return e->object();
    }

    static void install_handler(unsigned int interrupt);

private:
    Type_Id _type;
    unsigned int _unit;
    void * _object;
    unsigned int _interrupt;
    bool _busy;
    List::Element _link;

    static List _devices;
};

__END_SYS

#endif
