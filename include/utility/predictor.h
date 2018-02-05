// EPOS Predictor Declarations

#ifndef __predictor_h
#define __predictor_h

__BEGIN_UTIL

class Model_Common
{
public:
    // Model Types
    enum Type {
        NONE    = 0, // don't use this value
        LINEAR  = 1,
    };
protected:
    Model_Common(const unsigned char & type) : _type(type) {}
public:
    unsigned char type() const { return _type; }
private:
    unsigned char _type;
} __attribute__((packed));


template<typename T, typename V>
class Linear_Model : public Model_Common
{
    typedef T Time;
    typedef V Value;
public:
    enum{ TYPE = Model_Common::LINEAR };

    Linear_Model(const Value & a = 0, const Value & b = 0, const Time & t0 = 0)
    : Model_Common(TYPE), _a(a), _b(b), _t0(t0) {}

    Value operator()(const Time & t) { return (_a*(t-_t0) + _b); }

    Value a() const { return _a; }
    void a(const Value & a)  { _a = a; }

    Value b() const { return _b; }
    void b(const Value & b) { _b = b; }

    Time t0() const { return _t0; }
    void t0(const Time & t0) { _t0 = t0; }

private:
    Value _a;
    Value _b;
    Time _t0;
} __attribute__((packed));


// Entry Types
enum Entry_Types { TEMPORAL, NON_TEMPORAL };

template<Entry_Types K, typename V, typename ...O> struct Entry : V::NEVER { };

// Entry for temporal predictors
template<typename T, typename V>
struct Entry<TEMPORAL, T, V>
{
    Entry(const T & time=0, const V & value=0) : _time(time), _value(value) {}
    V value() const { return _value; }
    V time() const { return _time; }
private:
    T _time;
    V _value;
};

// Entry for non-temporal predictors
template<typename V>
struct Entry<NON_TEMPORAL, V>
{
    Entry(const V & value=0) : _value(value) {}
    V value() const { return _value; }
private:
    V _value;
};


class Predictor_Common
{
public:
    // Predictor Types
    enum Type {
        NONE    = 0, // don't use this value
        DBP     = (1 << 0),
        STDBP   = (1 << 1),
    };
protected:
    Predictor_Common(const unsigned char & type) : _type(type) {}
public:
    unsigned char type() const { return _type; }
private:
    unsigned char _type;
};

class Predictor_Dummy
{
public:
    typedef void * Model;
    struct Configuration {};
    enum { TYPE = Predictor_Common::NONE };
};

template<typename E, unsigned int W=10>
class Historical_Predictor : public Predictor_Common
{
public:
    Historical_Predictor(const unsigned char & type, unsigned int size = W) : Predictor_Common(type), history(new Cirular_Queue<E,W>(size)) {}

private:
    template<typename El, unsigned int SIZE>
    class Cirular_Queue
    {
        typedef El Element;
    public:
        Cirular_Queue(unsigned int size_limit = SIZE) : _size(0), _size_limit(size_limit <= SIZE ? size_limit : SIZE), _head(-1), _tail(-1) { }

        const Element & insert(const Element & el){
            if(full()) {
                _head = (_head + 1) % _size_limit;
                _tail = (_tail + 1) % _size_limit;
            } else {
                if(empty())
                    _head = _tail = 0;
                else
                    _tail = (_tail + 1) % _size_limit;
                _size++;
            }
            _queue[_tail] = el;
            return el;
        }

        unsigned int size() { return _size; }
        unsigned int max_size() { return _size_limit; }

        const Element & head() { return _queue[_head]; }
        const Element & tail() { return _queue[_tail]; }

        const Element & operator[](unsigned int index) {
            return _queue[(_head + index) % _size_limit];
        }

        bool empty() { return (_head == static_cast<unsigned int>(-1) && _tail == static_cast<unsigned int>(-1)); }
        bool full()  { return (_tail + 1) % _size_limit == _head ? true : false; }

        void clear() {
            _size = 0;
            _head = _tail = -1;
        }
    private:
        Element _queue[SIZE];
        unsigned int _size;
        unsigned int _size_limit;
        unsigned int _head;
        unsigned int _tail;
    };

protected:
    template<typename ...O>
    void store(const O & ...o) { history->insert(E(o...)); }

    Cirular_Queue<E,W> * history;
};

template<typename S, unsigned int W_MAX=100>
class DBP : public Historical_Predictor<Entry<Entry_Types::TEMPORAL, typename S::Microsecond, typename S::Value>, W_MAX>
{
    typedef typename S::Microsecond Time;
    typedef typename S::Value Value;
    typedef Historical_Predictor<Entry<Entry_Types::TEMPORAL, Time, Value>, W_MAX> Base;
    using Base::history;
    using Base::store;

public:
    typedef Linear_Model<Time, float> Model;

    enum{ TYPE = Predictor_Common::DBP };

    template<unsigned int CW, unsigned int CL, Value CR=0, Value CA=0 , Time CT=0>
    struct Configuration
    {
        enum Parameters {
            W = CW, // window size
            L = CL, // number of edge points to calculate the coefficients.
            R = CR, // relative tolerance
            A = CA, // absolute tolerance
            T = CT, // time tolerance
        };

        Configuration(unsigned int _w=CW, unsigned int _l=CL, Value _r=CR, Value _a=CA, Time _t=CT) : w(_w), l(_l), r(_r), a(_a), t(_t) {}

        template<typename Config>
        Configuration(const Config & conf) : w(conf.w), l(conf.l), r(conf.r), a(conf.a), t(conf.t) {}

        unsigned int w;
        unsigned int l;
        Value r;
        Value a;
        Time t;
    } __attribute__((packed));

public:
    DBP(unsigned int w, unsigned int l, Value _r=0, Value _a=0, Time _t=0) : Base(TYPE, w), _l(l), _rel_err(_r), _abs_err(_a), _t_err(_t), _model(0) { }

    template<typename Config>
    DBP(const Config & config) : Base(TYPE, config.w), _l(config.l), _rel_err(config.r), _abs_err(config.a), _t_err(config.t), _model(0) { }

    template<typename Config>
    DBP() :  Base(TYPE, Config::W), _l(Config::L), _rel_err(Config::R), _abs_err(Config::A), _t_err(Config::T), _model(0) { }

    template<typename ...O>
    Value predict(const Time & t, const O & ...o) const {
        if(_model)
            return (*_model)(t, o...);
        else if(!history->empty())
            return (Value)history->tail().value();
        else
            return 0;
    }

    void update(const Time & t, const Value & v) { store(t, v); }

    bool trickle(S * sensed) {
        update(sensed->_time, sensed->_value);

        if(!_model) {
            if(history->full()){
                build_model();
                return false;
            }
        } else {
            float predicted = predict(sensed->_time);
            float max_acceptable_error = max( abso(((float)sensed->_value * (float)_rel_err)/100), (float)_abs_err );
            float error = abso( (float)sensed->_value - predicted );

            if(error > max_acceptable_error){
                build_model();
                return false;
            }
        }

        return true;
    }

    void model(const Model & model) { (*_model) = model; }
    Model & model() const { return *_model; }

    //TODO
    template<typename C>
    void configure(const C & conf) {  }

private:
    template<typename T>
    T abso(const T & a){ return (a < 0) ? -a : a; }

    template<typename T>
    const T & max(const T & a, const T & b) { return (a >= b) ? a : b; }

    template<typename T>
    const T & min(const T & a, const T & b) { return (a <= b) ? a : b; }

protected:
    void build_model(){
        assert(history->full());

        if(!_model)
            _model = new Model();

        float avg_oldest = 0;
        float avg_recent = 0;

        _model->t0((*history)[0].time());

        for(unsigned int i = 0; i < _l; i++)
            avg_oldest += (*history)[i].value();
        avg_oldest /= _l;

        for(unsigned int i = 0; i < _l; i++)
            avg_recent += (*history)[history->size()-1-i].value();
        avg_recent /= _l;

        float t_oldest = ( (*history)[0].time() + (*history)[_l-1].time() )/2;
        float t_recent = ( (*history)[history->size()-1].time() + (*history)[history->size()-1-(_l-1)].time() )/2;

        _model->a((avg_recent - avg_oldest)/(t_recent - t_oldest));
        _model->b(avg_oldest);
    }

private:
    unsigned int _l;
    Value _rel_err;
    Value _abs_err;
    Time _t_err;
    Model * _model;
};

__END_UTIL

#endif
