#ifndef HEADER_KCON_ALLOCATOR
#define HEADER_KCON_ALLOCATOR

#include "types.h"
#include <cstdint>
#include <initializer_list>
#include <set>

struct OutOfMemory {};

struct FreeCell
{
    FreeCell* next;
    slot_t blank;
};

ASSERT( sizeof(FreeCell) == sizeof(Cell<slot_t,slot_t>) );

class SimpleAllocator
{
    const size_t _ncells;
    FreeCell* _page;
    FreeCell* _free_list;
    size_t _gc_count;

    static void _mark( std::set<void*>& live, pcell_t pcell );
public:
    SimpleAllocator( size_t ncells )
        : _ncells( ncells ), _page( new FreeCell[ncells] ), _free_list( 0 ), _gc_count( 0 )
    {
        gc({});
        _gc_count = 0;
    }

    void gc( const std::initializer_list<pcell_t*>& roots );

    size_t gc_count() const { return _gc_count; }

    void* allocate( const std::initializer_list<pcell_t*>& roots )
    {
        if( _free_list == 0 )
            gc( roots );

        void* p = _free_list;
        _free_list = _free_list->next;
        return p;
    }

    size_t num_allocated() const
    {
        size_t n = _ncells;
        for( const FreeCell* p = _free_list; p != 0; p = p->next )
            --n;

        return n;
    }
};

inline void* operator new( size_t size, SimpleAllocator& allocator, const std::initializer_list<pcell_t*>& roots={} )
{
    assert( size == sizeof(Cell<slot_t,slot_t>), "SimpleAllocator can only allocate cells of a fixed size" );

	return allocator.allocate( roots );
}

typedef SimpleAllocator Allocator;

#endif
