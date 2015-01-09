#include "shell.h"

using namespace kcon;
using namespace elpa;

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
auto KConShellManager<System, SchemeT, AllocatorT>::readers() -> const Readers&
{
    static Readers readers;
    return readers;
}

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
auto KConShellManager<System, SchemeT, AllocatorT>::macros() -> const Macros&
{
    static Macros macros;
    return macros;
}

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
auto KConShellManager<System, SchemeT, AllocatorT>::operators() -> const std::vector<char>&
{
    static std::vector<char> ops = {'!'};
    return ops;
}

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
auto KConShellManager<System, SchemeT, AllocatorT>::process_operator( char op, elem_t elem ) -> elem_t
{
    if( op == '!' )
    {
        return elem;
    }

    return elem_t();
}

#include "../elpa/runtime.h"
template class KConShellManager<Debug, SimpleScheme, TestAllocator>;
template class KConShellManager<Safe, OptScheme, OptAllocator>;
template class KConShellManager<Fast, OptScheme, OptAllocator>;
