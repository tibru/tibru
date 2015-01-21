#ifndef OHNO_SHELL_H_INCLUDED
#define OHNO_SHELL_H_INCLUDED

#include "../kcon/shell.h"

namespace ohno
{

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
class OhNoShellManager : public kcon::KConShellManager<System, SchemeT, AllocatorT>
{
    typedef kcon::KConShellManager<System, SchemeT, AllocatorT> Base;
public:
    OhNoShellManager( size_t ncells )
        : Base( ncells ) {}

    auto process_command( const std::string& cmd, elpa_istream<System, SchemeT, AllocatorT>& eis, elpa_ostream<System, SchemeT>& eos, bool noisy ) -> bool;
    void print_commands( elpa_ostream<System, SchemeT>& eos ) const;
};

}   //namespace

#endif // OHNO_SHELL_H_INCLUDED
