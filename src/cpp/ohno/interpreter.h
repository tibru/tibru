#ifndef OHNO_INTERPRETER_H_INCLUDED
#define OHNO_INTERPRETER_H_INCLUDED

#include "../kcon/interpreter.h"

namespace ohno {

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
class OhNoShellManager;

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
class OhNoInterpreter : public kcon::KConInterpreter<System, SchemeT, AllocatorT>
{
public:
};

}   //namespace

#endif // INTERPRETER_H_INCLUDED
