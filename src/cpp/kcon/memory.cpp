#include "memory.h"
#include "runtime.h"
#include <map>

using namespace kcon;

/** TestAllocator */

template<class Scheme>
void TestAllocator<Scheme>::gc( const Roots& roots )
{
    ++_gc_count;

    std::set<pcell_t> all;
    all.swap( _allocated );

    for( auto r : roots )
        if( r->is_pcell() )
            _mark( _allocated, r->pcell() );

    for( auto p : all )
        if( _allocated.find( p ) == _allocated.end() )
            delete p;

    if( _allocated.size() == _ncells )
        throw Error<Runtime,OutOfMemory>( "Out of memory" );
}



template<class Scheme>
void TestAllocator<Scheme>::_shift( const Roots& roots )
{
    std::set<pcell_t> all;
    all.swap( _allocated );

    std::map<pcell_t,Cell*> old_to_new;

    for( auto p : all )
    {
        auto q = new Cell( 199, 201 );
        _allocated.insert( q );
        old_to_new[p] = q;
    }

    auto move = [&old_to_new]( elem_t e ) { return e.is_pcell() && (e != null<elem_t>()) ? old_to_new[e.pcell()] : e; };

    for( auto p : old_to_new )
    {
        new (p.second) Cell( move( p.first->head() ), move( p.first->tail() ) );
        delete p.first;
    }

    for( auto r : roots )
        if( r->is_pcell() )
            *r = move( *r ).pcell();
}

template<class Scheme>
void TestAllocator<Scheme>::_mark( std::set<pcell_t>& live, pcell_t p )
{
    if( live.find( p ) == live.end() )
    {
        live.insert( p );

        assert( p->head() != null<elem_t>(), "Found null head during GC" );
        if( p->head().is_pcell() )
            _mark( live, p->head().pcell() );

        assert( p->tail() != null<elem_t>(), "Found null tail during GC" );
        if( p->tail().is_pcell() )
            _mark( live, p->tail().pcell() );
    }
}

/** SimpleAllocator */

template<class Scheme>
void SimpleAllocator<Scheme>::_mark( std::set<pcell_t>& live, pcell_t p )
{
    if( live.find( p ) == live.end() )
    {
        live.insert( p );

        if( p->head().is_pcell() )
            _mark( live, p->head().pcell() );

        if( p->tail().is_pcell() )
            _mark( live, p->tail().pcell() );
    }
}

template<class Scheme>
void SimpleAllocator<Scheme>::gc( const Roots& roots )
{
    ++_gc_count;

    std::set<pcell_t> live;
    for( auto r : roots )
        if( r->is_pcell() )
            _mark( live, r->pcell() );

    _free_list = 0;
    for( size_t i = 0; i != _ncells; ++i )
    {
        auto p = reinterpret_cast<Cell*>( &_page[i] );
        if( live.find( p ) == live.end() )
            _free_list = new (p) FreeCell{ _free_list, 0 };
    }

    if( _free_list == 0 )
        throw Error<Runtime,OutOfMemory>( "Out of memory" );
}

template class TestAllocator<SimpleScheme>;
template class SimpleAllocator<SimpleScheme>;
