#ifndef KCON_RUNTIME_H
#define KCON_RUNTIME_H

#include "container/kstack.h"
#include "stream.h"

namespace kcon {

struct Runtime;

template<class Schem, template<class> class Alloc>
struct Interpreter
{
    typedef Schem Scheme;
    typedef Alloc<Scheme> Allocator;

    template<class T> using kstack = container::kstack<Scheme,T>;
    using kostream = kcon::kostream<Scheme>;

    static elem_t parse( Allocator& allocator, const std::string& in )
    {
        std::istringstream iss( in );
        elem_t elem = null<elem_t>();
        kistream<Scheme,Allocator>( iss, allocator ) >> elem;
        return elem;
    }
};

}   //namespace

#endif
