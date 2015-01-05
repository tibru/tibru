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

    KConInterpreter( size_t ncells )
        : InterpreterBase<System, SchemeT, AllocatorT>( ncells ) {}

    typedef KConShellManager<System, SchemeT, AllocatorT> ShellManager;

    bool is_valid_operator( char op ) const { return op == '!'; }
    auto process_operator( char op, elem_t elem ) -> elem_t
    {
        return elem;
    }
};

}   //namespace

#endif // HEADER_KCON_INTERPRETER
