#include "interpreter.h"

using namespace kcon;
using namespace elpa;

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
auto KConInterpreter<System, SchemeT, AllocatorT>::constant( elem_t elem ) -> elem_t
{
    return elem;
}

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
auto KConInterpreter<System, SchemeT, AllocatorT>::_select( pcell_t path, elem_t target ) -> elem_t
{
    while( path != 0 )
    {
        System::check( path->head().is_pcell(), "Path tails count must be of the form [b b ...]" );

        pcell_t tails = path->head().pcell();

        byte_t hcount;
        if( path->tail().is_byte() )
        {
            hcount = path->tail().byte();
            path = 0;
        }
        else if( path->tail().is_pcell() )
        {
            path = path->tail().pcell();
            System::check( path->head().is_byte(), "Path head count must be a byte" );

            hcount = path->head().byte();

            System::check( path->head().is_byte(), "Path tail count must not be a byte" );
            path = path->tail().pcell();
        }
        else
            System::assert( false, "/ Path was neither cell nor byte" );

        size_t tcount = 0;
        int scale = 0;
        while( true )
        {
            System::check( tails->head().is_byte(), "Path tails count to be of the form [b b ...]" );
            tcount += (tails->head().byte() << scale);
            scale += 8;
            System::check( scale != sizeof(void*) * 8, "Path tail count overflow" );

            if( tails->tail().is_byte() )
            {
                tcount += (tails->tail().byte() << scale);
                break;
            }

            System::assert( tails->tail().is_pcell(), "Tails tail expected to be pcell" );
            tails = tails->tail().pcell();
        }

        while( tcount-- > 0 )
        {
            System::check( target.is_pcell(), "/ tried to access tail of a byte" );
            target = target.pcell()->tail();
        }

        while( hcount-- > 0 )
        {
            System::check( target.is_pcell(), "/ tried to access head of a byte" );
            target = target.pcell()->head();
        }
    }

    return target;
}

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
auto KConInterpreter<System, SchemeT, AllocatorT>::select( elem_t elem ) -> elem_t
{
    System::check( elem.is_pcell(), "/ operates only on pairs" );
    System::check( elem.pcell()->tail().is_pcell(), "/ requires paths of the form [([] b)+]" );
    return _select( elem.pcell()->tail().pcell(), elem.pcell()->head() );
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
