#ifndef KCON_INTERPRETER_H
#define KCON_INTERPRETER_H

#include "../elpa/interpreter.h"

using namespace elpa;

namespace kcon {

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
class KConInterpreter : public InterpreterBase<System, SchemeT, AllocatorT>
{
public:
    KConInterpreter( size_t ncells )
        : InterpreterBase<System, SchemeT, AllocatorT>( ncells ) {}

    bool is_valid_op( char op ) const { return false; }
};

}   //namespace

#endif // HEADER_KCON_INTERPRETER
