#include "Allocator.h"

using namespace kcon;

void SimpleAllocator::_mark( std::set<pcell_t>& live, pcell_t p )
{
    if( live.find( p.addr() ) != live.end() )
        return;

    live.insert( p.addr() );

    switch( p->typecode() )
    {
		case CellType<pcell_t,pcell_t>::TYPECODE:
		{
			_mark( live, p->head().pcell() );
			_mark( live, p->tail().pcell() );
			break;
		}
		case CellType<pcell_t,value_t>::TYPECODE:
		{
		    _mark( live, p->head().pcell() );
		    break;
		}
		case CellType<value_t,pcell_t>::TYPECODE:
		{
		    _mark( live, p->tail().pcell() );
		    break;
		}
		case CellType<value_t,value_t>::TYPECODE:
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

    std::set<pcell_t> live;
    for( auto r : roots )
        _mark( live, *r );

    _free_list = 0;
    for( size_t i = 0; i != _ncells; ++i )
    {
        auto p = reinterpret_cast<Cell*>( &_page[i] );
        if( live.find( p ) == live.end() )
            _free_list = new (p) FreeCell( _free_list );
    }

    if( _free_list == 0 )
        throw Error<Runtime,OutOfMemory>( "Out of memory" );
}
