#ifndef KCON_RUNTIME_H
#define KCON_RUNTIME_H

#include "container/kstack.h"
#include "stream.h"

namespace kcon {

struct Runtime;

template<class Scheme, template<class> class AllocatorTemplate>
struct Interpreter
{
    using Allocator = AllocatorTemplate<Scheme>;

    template<class T> using kstack = container::kstack<Scheme,T>;
    using kostream = kcon::kostream<Scheme>;
    using kistream = kcon::kistream<Scheme,Allocator>;
};

}   //namespace

#endif
