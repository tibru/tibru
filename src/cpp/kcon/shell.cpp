#include "shell.h"

using namespace kcon;
using namespace elpa;

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
auto KConShellManager<System, SchemeT, AllocatorT>::is_valid_operator( char op ) const -> bool
{
    return op == '!';
}

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
auto KConShellManager<System, SchemeT, AllocatorT>::process_operator( char op, elem_t elem, elpa_istream& in, elpa_ostream& out ) -> bool
{
    if( op == '!' )
    {
        in >> nomoreinput;
        out << elem << std::endl;
        return true;
    }

    return true;
}

#include "../elpa/runtime.h"
template class KConShellManager<elpa::Debug, elpa::SimpleScheme, elpa::SimpleAllocator>;
