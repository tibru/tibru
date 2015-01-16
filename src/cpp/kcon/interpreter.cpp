#include "interpreter.h"

using namespace kcon;
using namespace elpa;

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
auto KConInterpreter<System, SchemeT, AllocatorT>::constant( elem_t elem ) -> elem_t
{
    return elem;
}

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
auto KConInterpreter<System, SchemeT, AllocatorT>::_select( elem_t target, pcell_t path ) -> elem_t
{
    while( path != 0 )
    {
        pcell_t tails = path->head().pcell( "Path tails count must be cells" );

        byte_t hcount;
        if( path->tail().is_byte() )
        {
            hcount = path->tail().byte();
            path = 0;
        }
        else
        {
            System::assert( path->tail().is_pcell(), "Path was neither cell nor byte" );

            path = path->tail().pcell();

            hcount = path->head().byte( "Path head count must be a byte" );

            path = path->tail().pcell( "Path tail count must not be a byte" );
        }

        size_t tcount = 0;
        int scale = 0;
        while( true )
        {
            tcount += (tails->head().byte( "Path tails count must be of the form [b b ...]" ) << scale);
            scale += 8;
            System::check( scale != sizeof(void*) * 8, "Path tail count overflow" );

            if( tails->tail().is_byte() )
            {
                tcount += (tails->tail().byte() << scale);
                break;
            }

            tails = tails->tail().pcell( "Tails tail expected to be pcell" );
        }

        while( tcount-- > 0 )
            target = target.pcell( "Tried to access tail of a byte" )->tail();

        while( hcount-- > 0 )
            target = target.pcell( "Tried to access head of a byte" )->head();
    }

    return target;
}

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
auto KConInterpreter<System, SchemeT, AllocatorT>::select( elem_t elem ) -> elem_t
{
    pcell_t p = elem.pcell( "/ operates only on cells" );

    return _select( p->head(), p->tail().pcell( "/ requires paths of the form [([] b)+]" ) );
}

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
auto KConInterpreter<System, SchemeT, AllocatorT>::_ifcell( pcell_t choices, elem_t cond ) -> elem_t
{
    if( cond.is_pcell() )
        return choices->tail();

    System::assert( cond.is_byte(), "If condition was neither cell nor byte" );
    return choices->head();
}

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
auto KConInterpreter<System, SchemeT, AllocatorT>::ifcell( elem_t elem ) -> elem_t
{
    pcell_t p = elem.pcell( "? operates only on cells" );

    return _ifcell( p->head().pcell( "? Requires two choices not a byte"), p->tail() );
}

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
auto KConInterpreter<System, SchemeT, AllocatorT>::reduce( elem_t elem ) -> elem_t
{
    pcell_t p = elem.pcell( "@ operates only on cells" );
    byte_t code = p->head().byte( "@ requires head element to be 0 or 1" );
    if( code == 0 )
        return constant( p->tail() );
    else if( code == 1 )
        return select( p->tail() );
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
