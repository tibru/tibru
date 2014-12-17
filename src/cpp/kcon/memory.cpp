#include "memory.h"
#include "runtime.h"

using namespace kcon;

template<class Scheme>
auto SimpleAllocator<Scheme>::_moved( elem_t e ) -> elem_t
{
    return e;
}

template<class Scheme>
auto SimpleAllocator<Scheme>::_move( const Roots& roots ) -> void
{
    std::vector<Cell> new_page = _page;

    //update child nodes
    for( auto p = new_page.begin(); p != new_page.end(); ++p )
        new (&*p) Cell( _moved( p->head() ), _moved( p->tail() ) );

    //update free set
    for( typename std::set<Cell*>::iterator p = _free_set.begin(); p != _free_set.end(); ++p )
        *p = _moved( *p ).pcell();

    //update roots
    for( auto p = roots.begin(); p != roots.end(); ++p )
        *p = _moved( **p );

    _page = new_page;
}

template<class Scheme>
auto SimpleAllocator<Scheme>::_mark( std::set<pcell_t>& live, pcell_t p ) -> void
{
    if( live.find( p ) != live.end() )
        return;

    live.insert( p );

    if( p->head().is_pcell() )
        _mark( live, p->head().pcell() );

    if( p->tail().is_pcell() )
        _mark( live, p->tail().pcell() );
}

template<class Scheme>
auto SimpleAllocator<Scheme>::gc( const Roots& roots ) -> void
{
    ++_gc_count;

    std::set<pcell_t> live;
    for( auto r : roots )
        _mark( live, *r );

    _free_set.clear();
    for( size_t i = 0; i != _ncells; ++i )
    {
        auto p = &_page[i];
        if( live.find( p ) == live.end() )
            _free_set.insert( p );
    }

    if( _free_set.empty() )
        throw Error<Runtime,OutOfMemory>( "Out of memory" );
}

template auto SimpleAllocator<SimpleScheme>::_move( const Roots& roots ) -> void;
template auto SimpleAllocator<SimpleScheme>::_mark( std::set<pcell_t>&, pcell_t ) -> void;
template auto SimpleAllocator<SimpleScheme>::gc( const Roots& ) -> void;
