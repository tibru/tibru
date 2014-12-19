#include "memory.h"
#include "runtime.h"
#include <map>

using namespace kcon;

<<<<<<< HEAD
/** TestAllocator */

template<class System, MetaScheme class SchemeT>
void TestAllocator<System, SchemeT>::gc( const Roots& roots )
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



template<class System, MetaScheme class SchemeT>
void TestAllocator<System, SchemeT>::_shift( const Roots& roots )
=======
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
>>>>>>> 4edd44c6763ff556930f29ca9467a9495fdb8e21
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

    auto move = [&old_to_new]( elem_t e ) { return e.is_pcell() ? old_to_new[e.pcell()] : e; };

    for( auto p : old_to_new )
    {
        new (p.second) Cell( move( p.first->head() ), move( p.first->tail() ) );
        delete p.first;
    }

    for( auto r : roots )
        if( r->is_pcell() )
            *r = move( *r ).pcell();
}

template<class System, MetaScheme class SchemeT>
void TestAllocator<System, SchemeT>::_mark( std::set<pcell_t>& live, pcell_t p )
{
    if( live.find( p ) == live.end() )
    {
        live.insert( p );

        System::assert( p->head().is_def(), "Found undef head during GC" );
        if( p->head().is_pcell() )
            _mark( live, p->head().pcell() );

        System::assert( p->tail().is_def(), "Found undef tail during GC" );
        if( p->tail().is_pcell() )
            _mark( live, p->tail().pcell() );
    }
}

/** SimpleAllocator */

template<class System, MetaScheme class SchemeT>
void SimpleAllocator<System, SchemeT>::_mark( std::set<pcell_t>& live, pcell_t p )
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

template<class System, MetaScheme class SchemeT>
void SimpleAllocator<System, SchemeT>::gc( const Roots& roots )
{
    ++_gc_count;

    std::set<pcell_t> live;
    for( auto r : roots )
        if( r->is_pcell() )
            _mark( live, r->pcell() );

    _free_set.clear();
    for( size_t i = 0; i != _ncells; ++i )
    {
        auto p = &_page[i];
        if( live.find( p ) == live.end() )
<<<<<<< HEAD
            _free_list = new (p) FreeCell{ _free_list, 0 };
=======
            _free_set.insert( p );
>>>>>>> 4edd44c6763ff556930f29ca9467a9495fdb8e21
    }

    if( _free_set.empty() )
        throw Error<Runtime,OutOfMemory>( "Out of memory" );
}

<<<<<<< HEAD
template class TestAllocator< Debug, SimpleScheme >;
template class SimpleAllocator< Debug, SimpleScheme >;
=======
template auto SimpleAllocator<SimpleScheme>::_move( const Roots& roots ) -> void;
template auto SimpleAllocator<SimpleScheme>::_mark( std::set<pcell_t>&, pcell_t ) -> void;
template auto SimpleAllocator<SimpleScheme>::gc( const Roots& ) -> void;
>>>>>>> 4edd44c6763ff556930f29ca9467a9495fdb8e21
