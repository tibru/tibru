#ifndef HEADER_KCON_ALLOCATOR
#define HEADER_KCON_ALLOCATOR

#include "types.h"
#include <cstdint>

struct OutOfMemory {};

struct FreeCell
{
    FreeCell* next;
    slot_t blank;
};

ASSERT( sizeof(FreeCell) == sizeof(Cell<slot_t,slot_t>) );

class SimpleAllocator
{
    FreeCell* _page;
    FreeCell* _free_list;

    void _gc()
    {
        throw Error<Runtime,OutOfMemory>( "Out of memory" );
    }
public:
    SimpleAllocator( size_t ncells )
        : _page( new FreeCell[ncells] ), _free_list( 0 )
    {
        for( size_t i = 0; i != ncells; ++i )
            _free_list = new (&_page[i]) FreeCell{ _free_list, 0 };
    }

    void* allocate()
    {
        if( _free_list == 0 )
            _gc();

        void* p = _free_list;
        _free_list = _free_list->next;
        return p;
    }
};

typedef SimpleAllocator Allocator;

inline void* operator new( size_t size, Allocator& allocator )
{
    assert( size == sizeof(Cell<slot_t,slot_t>), "Allocator can only allocate cells of a fixed size" );

	return allocator.allocate();
}

#endif
