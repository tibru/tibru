#ifndef KCON_INTERPRETER_TPP
#define KCON_INTERPRETER_TPP

#include "interpreter.h"

namespace kcon {

using namespace elpa;

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
auto KConInterpreter<System, SchemeT, AllocatorT>::_constant( elem_t env, elem_t k ) -> elem_t
{
    return k;
}

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
auto KConInterpreter<System, SchemeT, AllocatorT>::constant( elem_t elem ) -> elem_t
{
    return _constant( elem_t(), elem );
}

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
auto KConInterpreter<System, SchemeT, AllocatorT>::_parse_count( pcell_t tails ) -> size_t
{
    size_t count = 0;
    int scale = 0;
    while( true )
    {
        count += Scheme::byte_value( tails->head().byte( "Path tails count must be of the form [b b ...]" ) ) << scale;
        scale += 8;
        System::check( scale != sizeof(void*) * 8, "Path tail count overflow" );

        if( tails->tail().is_byte() )
            break;

        tails = tails->tail().pcell( "Tails tail expected to be pcell" );
    }

    count += Scheme::byte_value( tails->tail().byte() ) << scale;

    return count;
}

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
auto KConInterpreter<System, SchemeT, AllocatorT>::_parse_path_elem( pcell_t path, size_t& tcount, size_t& hcount ) -> pcell_t
{
    tcount = _parse_count( path->head().pcell( "Path tails count must be cells" ) );

    if( path->tail().is_byte() )
    {
        hcount = Scheme::byte_value( path->tail().byte() );
        return 0;
    }

    System::assert( path->tail().is_pcell(), "Path was neither cell nor byte" );

    path = path->tail().pcell();

    hcount = Scheme::byte_value( path->head().byte( "Path head count must be a byte" ) );

    return path->tail().pcell( "Path tail count must not be a byte" );
}

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
auto KConInterpreter<System, SchemeT, AllocatorT>::_select( elem_t env, pcell_t path ) -> elem_t
{
    while( path != 0 )
    {
        size_t tcount;
        size_t hcount;

        path = _parse_path_elem( path, tcount, hcount );

        while( tcount-- > 0 )
            env = env.pcell( "Tried to access tail of a byte" )->tail();

        while( hcount-- > 0 )
            env = env.pcell( "Tried to access head of a byte" )->head();
    }

    return env;
}

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
auto KConInterpreter<System, SchemeT, AllocatorT>::select( elem_t elem ) -> elem_t
{
    pcell_t p = elem.pcell( "/ operates only on cells" );

    return _select( p->head(), p->tail().pcell( "/ requires paths of the form [([] b)+]" ) );
}

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
auto KConInterpreter<System, SchemeT, AllocatorT>::_ifcell( elem_t env, pcell_t choices, elem_t cond ) -> elem_t
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

    return _ifcell( elem_t(), p->head().pcell( "? Requires two choices not a byte"), p->tail() );
}

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
auto KConInterpreter<System, SchemeT, AllocatorT>::_reduce( elem_t env, pcell_t expr ) -> elem_t
{
    uint8_t code = Scheme::byte_value( expr->head().byte( "@ requires expression code to be a byte" ) );
    elem_t params = expr->tail();

    if( code == 0 )
        return _constant( env, params );
    else if( code == 1 )
        return _select( env, params.pcell( "@ 1 requires cell based path" ) );
    else if( code == 2 )
    {
        pcell_t p = params.pcell( "@ 2 requires cell based choices and condition" );
        pcell_t choices = p->head().pcell( "@ 2 requires cell based choices" );
        elem_t cond = p->tail();
        return _ifcell( env, choices, cond );
    }
    else
        System::check( false, "@ requires expression code to be 0 or 1" );

    return elem_t();
}

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
auto KConInterpreter<System, SchemeT, AllocatorT>::reduce( elem_t elem ) -> elem_t
{
    pcell_t p = elem.pcell( "@ operates only on cells" );

    return _evaluate( p->head(), p->tail().pcell( "@ requires cell expression" ) );
}

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
auto KConInterpreter<System, SchemeT, AllocatorT>::_graft( elem_t env, elem_t elem, pcell_t path ) -> elem_t
{
    auto_root<elem_t> v( this->allocator(), env );
    auto_root<elem_t> e( this->allocator(), elem );
    auto_root<pcell_t> pth( this->allocator(), path );

    elpa_stack<elem_t> route( this->allocator() );
    auto_root<pcell_t> rpth( this->allocator(), 0 );
    auto_root<pcell_t> tcells( this->allocator() );

    while( pth != 0 )
    {
        size_t tcount;
        size_t hcount;

        tcells = pth->head().pcell( "Path tails count must be cells" );
        pth = _parse_path_elem( pth, tcount, hcount );

        if( rpth == 0 )
            rpth = this->allocator().new_Cell( elem_t( tcells ), Scheme::new_byte( hcount ) );
        else
            rpth = this->allocator().new_Cell( elem_t( tcells ), this->allocator().new_Cell( byte_t( hcount ), elem_t( rpth ) ) );

        while( tcount-- > 0 )
        {
            route.push( v.pcell( "Tried to access tail of a byte" )->head() );
            v = v.pcell()->tail();
        }

        while( hcount-- > 0 )
        {
            route.push( v.pcell( "Tried to access head of a byte" )->tail() );
            v = v.pcell()->head();
        }
    }

    v = e;

    while( rpth != 0 )
    {
        size_t tcount;
        size_t hcount;

        rpth = _parse_path_elem( rpth, tcount, hcount );

        while( hcount-- > 0 )
        {
            v = this->allocator().new_Cell( v, route.top() );
            route.pop();
        }

        while( tcount-- > 0 )
        {
            v = this->allocator().new_Cell( route.top(), v );
            route.pop();
        }
    }

    return v;
}

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
auto KConInterpreter<System, SchemeT, AllocatorT>::graft( elem_t elem ) -> elem_t
{
    pcell_t p = elem.pcell( "+ operates only on cells" );
    pcell_t params = p->tail().pcell( "+ requires path and element");

    return _graft( p->head(), params->head(), params->tail().pcell( "+ requires path to be a cell" ) );
}

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
auto KConInterpreter<System, SchemeT, AllocatorT>::_evaluate( elem_t env, pcell_t expr ) -> elem_t
{
    if( expr->head().is_byte() )
        return _reduce( env, expr );

    System::assert( expr->head().is_pcell(), "* expr head is neither cell nor byte" );

    auto_root<elem_t> v( this->allocator(), env );
    auto_root<pcell_t> exprs( this->allocator(), expr );

    elem_t t = _reduce( v, exprs->head().pcell() );

    for(exprs = exprs->tail().pcell( "* cons form requires at least 2 cell based expressions" );
        exprs->head().is_pcell();
        exprs = exprs->tail().pcell( "* cons form requires only cell based expressions" )
    )
        t = this->allocator().new_Cell( _reduce( v, exprs->head().pcell() ), t );

    return this->allocator().new_Cell( _reduce( v, exprs ), t );
}

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
auto KConInterpreter<System, SchemeT, AllocatorT>::evaluate( elem_t elem ) -> elem_t
{
    pcell_t p = elem.pcell( "* operates only on cells" );

    return _evaluate( p->head(), p->tail().pcell( "* requires cell expression" ) );
}

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
auto KConInterpreter<System, SchemeT, AllocatorT>::execute( elem_t state ) -> elem_t
{
    bool more = true;
    while( more )
        state = execute_trace( state, more );

    return state;
}

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
auto KConInterpreter<System, SchemeT, AllocatorT>::execute_trace( elem_t state, bool& more ) -> elem_t
{
    elem_t env = state.pcell( "! requires cell state" )->tail();
    elem_t stmt = state.pcell()->head();

    more = true;
    if( stmt.is_pcell() )
    {
        return _evaluate( env, stmt.pcell() );
    }
    else if( Scheme::byte_value( stmt.byte() ) == 0 )
    {
        more = false;
        return env;
    }
    else if( Scheme::byte_value( stmt.byte() ) == 1 )
    {
        pcell_t params = env.pcell( "! graft form requires environment, path and element" );
        env = params->head();
        params = params->tail().pcell( "! graft form requires path and element" );
        elem_t elem = params->head();
        pcell_t path = params->tail().pcell( "! graft form requires cell based path" );
        return _graft( env, elem, path );
    }
    else
        System::check( false, "! statement code must be 0" );

    return elem_t();
}

}   //namespace

#endif
