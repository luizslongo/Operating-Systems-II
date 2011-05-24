// EPOS Metaprograms

#ifndef __meta_h
#define __meta_h

__BEGIN_SYS

// IF metaprogram
template<bool condition, typename Then, typename Else>
struct IF 
{ typedef Then Result; };

template<typename Then, typename Else>
struct IF<false, Then, Else>
{ typedef Else Result; };

// IF metaprogram for integer
template<bool condition, int Then, int Else>
struct IF_INT
{ enum { Result = Then }; };

template<int Then, int Else>
struct IF_INT<false, Then, Else>
{ enum { Result = Else }; };


// EQUAL metaprogram
template<typename T1, typename T2>
struct EQUAL 
{ enum { Result = false }; };

template<typename T>
struct EQUAL<T, T>
{ enum { Result = true }; };


// LIST metaprogram
template <typename T1 = void, typename T2 = void, typename T3 = void,
	  typename T4 = void, typename T5 = void, typename T6 = void>
class LIST
{
private:
    typedef T1 Head;
    typedef LIST<T2, T3, T4, T5, T6> Tail;

public:
    enum { Length = Tail::Length + 1 };

    template<int Index, int Current = 0, bool Stop = (Index == Current)>
    struct Get
    { typedef typename Tail::template Get<Index, Current + 1>::Result
      Result; };

    template<int Index, int Current>
    struct Get<Index, Current, true>
    { typedef Head Result; };

    template<typename Type>
    struct Count
    { enum { Result = EQUAL<Head, Type>::Result
	     + Tail::template Count<Type>::Result }; };

    enum { Polymorphic = (int(Length) != int(Count<Head>::Result)) };
};

template <>
struct LIST<void, void, void, void, void, void>
{ 
    enum { Length = 0 }; 

    template<int Index, int Current = 0>
    struct Get
    { typedef void Result; };

    template<typename Type>
    struct Count
    { enum { Result = 0 }; };

    enum { Polymorphic = false };
};

__END_SYS

#endif
