#ifndef KCON_INTERPRETER_H
#define KCON_INTERPRETER_H

#include "../elpa/interpreter.h"

using namespace elpa;

namespace kcon {

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
class KConShellManager;

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
class KConInterpreter : public InterpreterBase<System, SchemeT, AllocatorT>
{
public:
    typedef SchemeT<System> Scheme;
    typedef typename Scheme::elem_t elem_t;
    typedef typename Scheme::byte_t byte_t;

    KConInterpreter( size_t ncells )
        : InterpreterBase<System, SchemeT, AllocatorT>( ncells ) {}

    typedef KConShellManager<System, SchemeT, AllocatorT> ShellManager;

    auto constant( elem_t elem, elem_t state ) -> elem_t;
    auto select( elem_t elem, elem_t state ) -> elem_t;
    auto reduce( elem_t elem, elem_t state ) -> elem_t;
    auto execute( elem_t state ) -> elem_t;
};

}   //namespace

#endif // HEADER_KCON_INTERPRETER
