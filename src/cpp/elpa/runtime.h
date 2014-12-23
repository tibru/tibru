#ifndef ELPA_RUNTIME_H
#define ELPA_RUNTIME_H

#include "container/elpa_stack.h"
#include "container/elpa_map.h"
#include "stream.h"
#include "interpreter.h"

namespace elpa {

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
    MetaAllocator class AllocatorT,
    MetaInterpreter class InterpreterT
>
struct Env
{
    using Scheme = SchemeT<System>;
    using Allocator = AllocatorT<System, SchemeT>;
    using Interpreter = InterpreterT<System, SchemeT, AllocatorT>;

    template<class T> using elpa_stack = container::elpa_stack<System, SchemeT, AllocatorT, T>;
    template<class K, class V> using elpa_map = elpa::container::elpa_map<System, SchemeT, AllocatorT, K, V>;
    using elpa_ostream = elpa::elpa_ostream<System, SchemeT>;
    using elpa_istream = elpa::elpa_istream<System, SchemeT, AllocatorT>;

    using byte_t = typename Scheme::byte_t;
    using pcell_t = typename Scheme::pcell_t;
    using elem_t = typename Scheme::elem_t;
};

}   //namespace

#endif
