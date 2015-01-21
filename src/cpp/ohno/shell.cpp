#include "shell.h"

using namespace ohno;

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
auto OhNoShellManager<System, SchemeT, AllocatorT>::process_command( const std::string& cmd, elpa_istream<System, SchemeT, AllocatorT>& eis, elpa_ostream<System, SchemeT>& eos, bool noisy ) -> bool
{
    if( cmd == "fn" )
    {

    }
    else
        return Base::process_command( cmd, eis, eos, noisy );

    return true;
}

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
void OhNoShellManager<System, SchemeT, AllocatorT>::print_commands( elpa_ostream<System, SchemeT>& eos ) const
{
    Base::print_commands( eos );
    eos << ":fn   - Create an oh-no function\n";
}

#include "../elpa/runtime.h"
template class OhNoShellManager<Debug, SimpleScheme, TestAllocator>;
template class OhNoShellManager<Safe, OptScheme, OptAllocator>;
template class OhNoShellManager<Fast, OptScheme, OptAllocator>;
