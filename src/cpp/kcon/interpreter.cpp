#include "interpreter.h"

using namespace kcon;
using namespace elpa;

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
auto KConInterpreter<System, SchemeT, AllocatorT>::constant( elem_t elem ) -> elem_t
{
    return elem;
}

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
auto KConInterpreter<System, SchemeT, AllocatorT>::select( elem_t elem ) -> elem_t
{
    System::check( elem.is_pcell(), "/ operates only on pairs" );
    return elem;
}

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
auto KConInterpreter<System, SchemeT, AllocatorT>::reduce( elem_t elem ) -> elem_t
{
    System::check( elem.is_pcell(), "@ operates only on pairs" );
    System::check( elem.pcell()->head().is_byte(), "@ requires head element to be 0 or 1" );
    byte_t code = elem.pcell()->head().byte();
    if( code == 0 )
        return constant( elem.pcell()->tail() );
    else if( code == 1 )
        return select( elem.pcell()->tail() );
    else
        System::check( false, "@ requires head element to be 0 or 1" );

    return elem_t();
}

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
auto KConInterpreter<System, SchemeT, AllocatorT>::execute( elem_t state ) -> elem_t
{
    System::assert( state.is_def(), "Undefined state passed to execute" );

    System::check( state.is_pcell(), "Illegal byte state for !" );

    return state;
}

#include "../elpa/runtime.h"
template class KConInterpreter<Debug, SimpleScheme, TestAllocator>;
template class KConInterpreter<Debug, SimpleScheme, SimpleAllocator>;
template class KConInterpreter<Safe, OptScheme, OptAllocator>;
template class KConInterpreter<Fast, OptScheme, OptAllocator>;
