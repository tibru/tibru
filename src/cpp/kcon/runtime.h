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

template<class Sys, template<class> class Schem, template<class, class> class AllocatorTemplate>
struct Env
{
    using Scheme = Schem<Sys>;
    using Allocator = AllocatorTemplate<Sys, Scheme>;

    template<class T> using kstack = container::kstack<Scheme,Allocator,T>;
    using kostream = kcon::kostream<Sys, Scheme>;
    using kistream = kcon::kistream<Sys, Scheme,Allocator>;
};

}   //namespace

#endif
