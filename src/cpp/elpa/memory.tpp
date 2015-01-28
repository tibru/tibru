#ifndef HEADER_ELPA_ALLOCATOR_TPP
#define HEADER_ELPA_ALLOCATOR_TPP

#include "memory.h"
#include "runtime.h"
#include <map>

namespace elpa {

/** TestAllocator */

template<class System, MetaScheme class SchemeT>
void TestAllocator<System, SchemeT>::gc()
{
    ++this->_gc_count;

    std::set<pcell_t> all;
    all.swap( _allocated );

    for( auto r : this->_elem_roots )
        if( r->is_pcell() )
            _mark( _allocated, r->pcell() );

    for( auto r : this->_pcell_roots )
        _mark( _allocated, *r );

    for( auto p : all )
        if( _allocated.find( p ) == _allocated.end() )
            delete p;

    if( _allocated.size() == this->_ncells )
        throw Error<Runtime,OutOfMemory>( "Out of memory" );
}



template<class System, MetaScheme class SchemeT>
void TestAllocator<System, SchemeT>::_shift()
{
    std::set<pcell_t> all;
    all.swap( _allocated );

    std::map<pcell_t,Cell*> old_to_new;

    for( auto p : all )
    {
        auto q = System::check_address( new Cell( elem_t(), elem_t() ) );
        _allocated.insert( q );
        old_to_new[p] = q;
    }

    auto move_elem = [&old_to_new]( elem_t e ) { return e.is_pcell() ? old_to_new[e.pcell()] : e; };
    auto move_pcell = [&old_to_new]( pcell_t p ) { return old_to_new[p]; };

    for( auto p : old_to_new )
    {
        new (p.second) Cell( move_elem( p.first->head() ), move_elem( p.first->tail() ) );
        new ((Cell*) p.first) Cell( elem_t(), elem_t() );
        delete p.first;
    }

    for( auto r : this->_elem_roots )
        if( r->is_pcell() )
            *r = move_elem( *r ).pcell();

    for( auto r : this->_pcell_roots )
        *r = move_pcell( *r );
}

template<class System, MetaScheme class SchemeT>
void TestAllocator<System, SchemeT>::_mark( std::set<pcell_t>& live, pcell_t p )
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
void SimpleAllocator<System, SchemeT>::gc()
{
    ++this->_gc_count;

    std::set<pcell_t> live;
    for( auto r : this->_elem_roots )
        if( r->is_pcell() )
            _mark( live, r->pcell() );

    for( auto r : this->_pcell_roots )
        _mark( live, *r );

    _free_list = 0;
    for( size_t i = 0; i != this->_ncells; ++i )
    {
        auto p = reinterpret_cast<Cell*>( &_page[i] );
        if( live.find( p ) == live.end() )
            _free_list = new (p) FreeCell{ _free_list, elem_t() };
    }

    if( _free_list == 0 )
        throw Error<Runtime,OutOfMemory>( "Out of memory" );
}

}   //namespace

#endif
