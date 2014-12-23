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
    using elpa_istream = elpa::elpa_istream<System, SchemeT, AllocatorT>;

    KConShellManager( size_t ncells )
        : elpa::ShellManagerBase<System, SchemeT, AllocatorT, KConInterpreter>( ncells ) {}

    bool is_valid_operator( char op ) const;

    auto process_operator( char op, elpa_istream& eis, std::ostream& out ) -> bool;
};

}   //namespace

#endif
