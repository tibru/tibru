#ifndef KCON_SHELL_H
#define KCON_SHELL_H

#include "../elpa/shell.h"
#include "interpreter.h"

namespace kcon
{

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
class KConShellManager : public elpa::ShellManagerBase<System, SchemeT, AllocatorT, KConInterpreter>
{
public:
    typedef SchemeT<System> Scheme;
    typedef typename Scheme::elem_t elem_t;
    typedef elpa::elpa_ostream<System, SchemeT> elpa_ostream;
    typedef elpa::elpa_istream<System, SchemeT, AllocatorT> elpa_istream;

    KConShellManager( size_t ncells )
        : elpa::ShellManagerBase<System, SchemeT, AllocatorT, KConInterpreter>( ncells ) {}

    bool is_valid_operator( char op ) const;

    auto process_operator( char op, elem_t elem, elpa_istream& in, elpa_ostream& out ) -> bool;
};

}   //namespace

#endif
