// EPOS Application-level Dynamic Memory Utility Declarations

#ifndef __malloc_h
#define __malloc_h

#include <application.h>

extern "C"
{
    inline void * malloc(size_t bytes) {
	return __SYS(Application)::heap()->alloc(bytes);
    }

    inline void * calloc(size_t n, unsigned int bytes) {
	return __SYS(Application)::heap()->calloc(n * bytes);
    }

    inline void free(void * ptr) {
	__SYS(Application)::heap()->free(ptr);
    }

    inline void * p_malloc(size_t bytes) {
        return __SYS(Application)::priority_heap()->alloc(bytes);
    }

    inline void * p_calloc(size_t n, unsigned int bytes) {
        return __SYS(Application)::priority_heap()->calloc(n * bytes);
    }

    inline void p_free(void * ptr) {
        __SYS(Application)::priority_heap()->free(ptr);
    }
}

inline bool to_prio_heap(unsigned int bytes, __SYS(alloc_priority) p){
    typedef __SYS(Application) Application;
    typedef __SYS(Traits)<__SYS(Machine)> Traits;

    bool ret = false;
    switch (p) {
        case __SYS(ALLOC_P_HIGH):
            if(bytes < (Traits::PRIORITY_HEAP_SIZE - Application::priority_heap()->allocated()))
                ret = true;
            else
                ret = false;
            break;
        case __SYS(ALLOC_P_LOW):
            if(bytes < (Traits::APPLICATION_HEAP_SIZE - Application::heap()->allocated()))
                ret = false;
            else
                ret = true;
            break;
        case __SYS(ALLOC_P_NORMAL):{
            unsigned int spm_used =
                    100 / (Traits::PRIORITY_HEAP_SIZE / Application::priority_heap()->allocated());
            unsigned int main_used =
                    100 / (Traits::APPLICATION_HEAP_SIZE / Application::heap()->allocated());

            //__SYS(kout) << "Main mem - size: " << Traits::APPLICATION_HEAP_SIZE
            //   << " allocated: " << Application::heap()->allocated()
            //   << " used: " << main_used << "%\n";

            //__SYS(kout) << "SPM mem - size: " << Traits::SPM_HEAP_SIZE
            //   << " allocated: " << Application::spm_heap()->allocated()
            //   << " used: " << spm_used << "%\n";

            bool spm_fit =
                    bytes < (Traits::PRIORITY_HEAP_SIZE - Application::priority_heap()->allocated());
            bool main_fit =
                    bytes < (Traits::APPLICATION_HEAP_SIZE - Application::heap()->allocated());
            bool spm_more_free = spm_used <= main_used;

            if(spm_more_free)
                ret = spm_fit;
            else
                ret = !main_fit;

            break;
        }
        default:
            break;
    }

    //__SYS(kout) << "Return " << ret << "\n";
    return ret;
}
inline bool from_prio_heap(void * ptr){
    typedef __SYS(Traits)<__SYS(Machine)> Traits;

    return (reinterpret_cast<unsigned int>(ptr) >= Traits::PRIORITY_HEAP_BASE_ADDR) &&
            (reinterpret_cast<unsigned int>(ptr) <= Traits::PRIORITY_HEAP_TOP_ADDR);
}

inline void * decide_malloc(unsigned int bytes,
        __SYS(alloc_priority) p = __SYS(ALLOC_P_NORMAL)) {
    if(to_prio_heap(bytes, p)){
        void *rtn = p_malloc(bytes);
        if(rtn == 0) rtn = malloc(bytes);
        return rtn;
    }
    else {
        void *rtn = malloc(bytes);
        if(rtn == 0) rtn = p_malloc(bytes);
        return rtn;
    }
}
inline void * decide_calloc(unsigned int n, unsigned int bytes,
        __SYS(alloc_priority) p = __SYS(ALLOC_P_NORMAL)) {
    if(to_prio_heap(n*bytes, p))
            return p_calloc(n, bytes);
        else
            return calloc(n, bytes);
}
inline void decide_free(void * ptr) {
    if(from_prio_heap(ptr))
        p_free(ptr);
    else
        free(ptr);
}

inline void * operator new(size_t bytes, __SYS(alloc_priority) p) {
    if(__SYS(Traits)<__SYS(Heap)>::priority_alloc)
        return decide_malloc(bytes, p);
    else
        return malloc(bytes);
}
inline void * operator new[](size_t bytes, __SYS(alloc_priority) p) {
    if(__SYS(Traits)<__SYS(Heap)>::priority_alloc)
        return decide_malloc(bytes, p);
    else
        return malloc(bytes);
}

inline void * operator new(size_t bytes) {
    if(__SYS(Traits)<__SYS(Heap)>::priority_alloc)
        return decide_malloc(bytes);
    else
        return malloc(bytes);
}
inline void * operator new[](size_t bytes) {
    if(__SYS(Traits)<__SYS(Heap)>::priority_alloc)
        return decide_malloc(bytes);
    else
        return malloc(bytes);
}
inline void operator delete(void * object) {
    if(__SYS(Traits)<__SYS(Heap)>::priority_alloc)
        return decide_free(object);
    else
        return free(object);
}
inline void operator delete[](void * object) {
    if(__SYS(Traits)<__SYS(Heap)>::priority_alloc)
        return decide_free(object);
    else
        return free(object);
}

#endif
