#ifndef ELPA_INTERPRETER_H
#define ELPA_INTERPRETER_H

#include "types.h"
#include "memory.h"

namespace elpa
{

#define MetaInterpreter template<class System, template<class> class SchemeT, template<class System, template<class> class SchemeT> class AllocatorT>

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
class InterpreterBase
{
    typedef AllocatorT<System, SchemeT> Allocator;

    Allocator _allocator;
public:
    InterpreterBase( size_t ncells )
        : _allocator( ncells ) {}

    Allocator& allocator() { return _allocator; }
};

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
struct NullInterpreter : InterpreterBase<System, SchemeT, AllocatorT>
{
    bool is_valid_op( char op ) const { return false; }
};

}   //namespace

#endif

