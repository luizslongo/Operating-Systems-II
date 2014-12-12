// EPOS Metaprograms

#ifndef __meta_h
#define __meta_h

__BEGIN_SYS

// Conditional Type
template<bool condition, typename Then, typename Else>
struct IF 
{ typedef Then Result; };

template<typename Then, typename Else>
struct IF<false, Then, Else>
{ typedef Else Result; };


// Conditional Integer
template<bool condition, int Then, int Else>
struct IF_INT
{ enum { Result = Then }; };

template<int Then, int Else>
struct IF_INT<false, Then, Else>
{ enum { Result = Else }; };


// SWITCH-CASE of Types
const int DEFAULT = ~(~0u >> 1); // Initialize with the smallest int

struct Nil_Case {};

template<int tag_, typename Type_, typename Next_ = Nil_Case>
struct CASE
{ 
    enum { tag = tag_ }; 
    typedef Type_ Type;
    typedef Next_ Next;
};

template<int tag, typename Case>
class SWITCH
{
    typedef typename Case::Next Next_Case;
    enum { 
        case_tag = Case::tag, 
        found = ( case_tag == tag || case_tag == DEFAULT  )
    };
public:
    typedef typename IF<found, typename Case::Type,
        typename SWITCH<tag, Next_Case>::Result>::Result Result;
};

template<int tag>
class SWITCH<tag, Nil_Case>
{
public:
    typedef Nil_Case Result;
};


// EQUALty of Types
template<typename T1, typename T2>
struct EQUAL 
{ enum { Result = false }; };

template<typename T>
struct EQUAL<T, T>
{ enum { Result = true }; };


// SIZEOF Type Package
template<typename ... Tn>
struct SIZEOF
{ static const unsigned int  Result = 0; };
template<typename T1, typename ... Tn>
struct SIZEOF<T1, Tn ...>
{ static const unsigned int Result = sizeof(T1) + SIZEOF<Tn ...>::Result ; };

// LIST of Types
template<typename ... Tn> class LIST;
template<typename T1, typename ... Tn>
class LIST<T1, Tn ...>
{
protected:
    typedef T1 Head;
    typedef LIST<Tn ...> Tail;

public:
    enum { Length = Tail::Length + 1 };

    template<int Index, int Current = 0, bool Stop = (Index == Current)>
    struct Get
    { typedef typename Tail::template Get<Index, Current + 1>::Result Result; };

    template<int Index, int Current>
    struct Get<Index, Current, true>
    { typedef Head Result; };

    template<typename Type>
    struct Count
    { enum { Result = EQUAL<Head, Type>::Result + Tail::template Count<Type>::Result }; };

    struct Recur: public Tail::Recur::Result
    { typedef Head Result; };

    enum { Polymorphic = (int(Length) != int(Count<Head>::Result)) };
};

template<>
class LIST<>
{ 
public:
    enum { Length = 0 }; 

    template<int Index, int Current = 0>
    struct Get
    { typedef void Result; };

    template<typename Type>
    struct Count
    { enum { Result = 0 }; };

    struct Recur
    { class Result {}; };

    enum { Polymorphic = false };
};


// LIST of Templates
template<template<typename T> class ... Tn> class TLIST;
template<template<typename T> class T1, template<typename T> class ... Tn>
class TLIST<T1, Tn ...>
{
public:
    enum { Length = TLIST<Tn ...>::Length + 1 };

    template<typename T>
    struct Recur: public T1<T>, public TLIST<Tn ...>::template Recur<T> {};
};

template<>
class TLIST<>
{
public:
    enum { Length = 0 };

    template<typename T>
    struct Recur {};
};

__END_SYS

#endif
