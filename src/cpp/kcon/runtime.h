#ifndef KCON_RUNTIME_H
#define KCON_RUNTIME_H

#include "container/kstack.h"
#include "stream.h"

namespace kcon {

struct Runtime;

template<bool AssertF=false>
struct Params
{
    static const bool AssertFlag = AssertF;

    template<bool flag> struct Assert : Params<flag> {};
};

template<class Params>
struct System
{
    static void assert( bool cond, const std::string& msg )
    {
        if( Params::AssertFlag && !cond )
            throw Error<Assertion>( msg );
    }

    template<class T>
    static T* check_address( T* p )
    {
        assert( reinterpret_cast<uintptr_t>(p) < MAX_POINTER, "Invalid address" );
        return p;
    }
};

typedef System< Params<>::Assert<false> > Debug;

template<
    class System,
    MetaScheme class SchemeT,
    MetaAllocator class AllocatorT
>
struct Env
{
    using Scheme = SchemeT<System>;
    using Allocator = AllocatorT<System, SchemeT>;

    template<class T> using kstack = container::kstack<System, SchemeT, AllocatorT, T>;
    using kostream = kcon::kostream<System, SchemeT>;
    using kistream = kcon::kistream<System, SchemeT, AllocatorT>;
};

}   //namespace

#endif
