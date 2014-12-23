#ifndef KCON_INTERPRETER_H
#define KCON_INTERPRETER_H

#include "../elpa/interpreter.h"

using namespace elpa;

namespace kcon {

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
class KConInterpreter : public InterpreterBase<System, SchemeT, AllocatorT>
{
    using elpa_istream = elpa::elpa_istream<System, SchemeT, AllocatorT>;
public:
    KConInterpreter( size_t ncells )
        : InterpreterBase<System, SchemeT, AllocatorT>( ncells ) {}

    struct ShellManager
    {
        bool is_valid_operator( char op ) const { return false; }

        auto process_operator( char op, elpa_istream& eis, KConInterpreter& interpreter, std::ostream& out ) -> bool;
    };
};

}   //namespace

#endif // HEADER_KCON_INTERPRETER
