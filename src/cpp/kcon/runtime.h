#ifndef KCON_RUNTIME_H
#define KCON_RUNTIME_H

#include "container/kstack.h"
#include "stream.h"

namespace kcon {

struct Runtime;

template<bool AssertFlag=false>
struct Params
{
    template<bool flag> struct Assert : Params<flag> {};

    static void assert( bool cond, const std::string& msg )
    {
        if( AssertFlag && !cond )
            throw Error<Assertion>( msg );
    }
};

typedef Params<>::Assert<false> Debug;

template<
    class System,
    template<class System> class SchemeT,
    template<class System, template<class> class SchemeTemplate> class AllocatorT
>
struct Env
{
    using Scheme = SchemeT<System>;
    using Allocator = AllocatorT<System, SchemeT>;

    template<class T> using kstack = container::kstack<Scheme,Allocator,T>;
    using kostream = kcon::kostream<System, Scheme>;
    using kistream = kcon::kistream<System, Scheme,Allocator>;
};

}   //namespace

#endif
