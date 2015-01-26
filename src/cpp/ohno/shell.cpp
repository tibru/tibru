#include "shell.h"

using namespace ohno;

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
auto OhNoShellManager<System, SchemeT, AllocatorT>::readers() -> const Readers&
{
    static Readers readers = Base::readers();

    return readers;
}

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
auto OhNoShellManager<System, SchemeT, AllocatorT>::macros() -> const Macros&
{
    static Macros macros;

    if( macros.size() == 0 )
    {
        macros = Base::macros();
        macros['%'] = []( Allocator& alloc, elem_t tail ) -> elem_t {
            auto_root<elem_t> e1( alloc, tail.pcell("% Requires at least a 2 element list")->head() );
            auto_root<elem_t> e2( alloc, tail.pcell()->tail().pcell("% Requires at least a 2 element list")->head() );

            elem_t r = alloc.new_Cell( e1, tail.pcell()->tail().pcell()->tail() );
            return alloc.new_Cell( e2, r );
        };
    }

    return macros;
}

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
void OhNoShellManager<System, SchemeT, AllocatorT>::print_help( elpa_ostream<System,SchemeT>& eos )
{
    Base::print_help( eos );
    eos << "% - Swap last two elements of a list\n";
}

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
template class OhNoShellManager<Debug, SimpleScheme, SimpleAllocator>;
template class OhNoShellManager<Safe, OptScheme, OptAllocator>;
template class OhNoShellManager<Fast, OptScheme, OptAllocator>;

