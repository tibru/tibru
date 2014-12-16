#ifndef KCON_RUNTIME_H
#define KCON_RUNTIME_H

namespace kcon {

struct Runtime;

template<class Schem, template<class> class Alloc>
struct Interpreter
{
    typedef Schem Scheme;
    typedef Alloc<Scheme> Allocator __attribute__((unused));
};

}   //namespace

#endif
