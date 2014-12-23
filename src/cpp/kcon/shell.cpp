#include "shell.h"

using namespace kcon;

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
auto KConShellManager<System, SchemeT, AllocatorT>::is_valid_operator( char op ) const -> bool
{
    return true;
}

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
auto KConShellManager<System, SchemeT, AllocatorT>::process_operator( char op, elpa_istream& eis, std::ostream& out ) -> bool
{
    eis >> nomoreinput;
    out << "OP " << op << std::endl;
    return true;
}

#include "../elpa/runtime.h"
template class KConShellManager<elpa::Debug, elpa::SimpleScheme, elpa::SimpleAllocator>;
