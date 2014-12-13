#include "Allocator.h"

using namespace kcon;

void SimpleAllocator::_mark( std::set<void*>& live, pcell_t pcell )
{
    if( live.find( pcell.addr() ) != live.end() )
        return;

    live.insert( pcell.addr() );

    switch( pcell.typecode() )
    {
		case Cell<pcell_t,pcell_t>::TYPECODE:
		{
			auto p = pcell.cast<pcell_t,pcell_t>();
			_mark( live, p->head );
			_mark( live, p->tail );
			break;
		}
		case Cell<pcell_t,value_t>::TYPECODE:
		{
		    auto p = pcell.cast<pcell_t,value_t>();
		    _mark( live, p->head );
		    break;
		}
		case Cell<value_t,pcell_t>::TYPECODE:
		{
		    auto p = pcell.cast<value_t,pcell_t>();
		    _mark( live, p->tail );
		    break;
		}
		case Cell<value_t,value_t>::TYPECODE:
		{
		    break;
		}
		default:
			throw Error<Runtime>( "mark dispatch failed" );
    }
}

void SimpleAllocator::gc( const Roots& roots )
{
    ++_gc_count;

    std::set<void*> live;
    for( auto r : roots )
        _mark( live, *r );

    _free_list = 0;
    for( size_t i = 0; i != _ncells; ++i )
    {
        void* p = &_page[i];
        if( live.find( p ) == live.end() )
            _free_list = new (p) FreeCell( _free_list );
    }

    if( _free_list == 0 )
        throw Error<Runtime,OutOfMemory>( "Out of memory" );
}
