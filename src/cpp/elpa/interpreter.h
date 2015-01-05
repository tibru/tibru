#ifndef ELPA_INTERPRETER_H
#define ELPA_INTERPRETER_H

#include "types.h"
#include "memory.h"
#include "stream.h"

namespace elpa
{

struct IllegalOp;

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
class NullShellManager;

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
class NullInterpreter : public InterpreterBase<System, SchemeT, AllocatorT>
{
public:
    NullInterpreter( size_t ncells )
        : InterpreterBase<System, SchemeT, AllocatorT>( ncells ) {}

    typedef NullShellManager<System, SchemeT, AllocatorT> ShellManager;
};

}   //namespace

#endif
