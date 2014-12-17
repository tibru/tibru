#ifndef HEADER_KCON_ALLOCATOR
#define HEADER_KCON_ALLOCATOR

#include "types.h"
#include <cstdint>
#include <cstdlib>
#include <initializer_list>
#include <set>
#include <vector>

namespace kcon {

struct OutOfMemory {};

template<class Scheme>
class SimpleAllocator
{
    typedef typename Scheme::value_t value_t;
    typedef typename Scheme::pcell_t pcell_t;
    typedef typename Scheme::elem_t elem_t;
    typedef typename Scheme::Cell Cell;

    const size_t _ncells;
    std::vector<Cell> _page;    //effectively const to avoid reallocation
    std::set<Cell*> _free_set;
    size_t _gc_count;

    static auto _mark( std::set<pcell_t>& live, pcell_t pcell ) -> void;

    SimpleAllocator( SimpleAllocator& );
    SimpleAllocator& operator=( const SimpleAllocator& );
public:
    typedef std::initializer_list<pcell_t*> Roots;

    SimpleAllocator( size_t ncells )
        : _ncells( ncells ), _page( ncells, Cell( 1, 1 ) ), _free_set(), _gc_count( 0 )
    {
        gc({});
        _gc_count = 0;
    }

    auto gc( const Roots& roots ) -> void;

    auto gc_count() const -> size_t { return _gc_count; }

    auto allocate( const Roots& roots ) -> void*
    {
        if( _free_set.empty() )
            gc( roots );

        void* p = *_free_set.begin();
        _free_set.erase( _free_set.begin() );
        return p;
    }

    auto num_allocated() const -> size_t
    {
        return _ncells - _free_set.size();
    }

    auto new_Cell( const elem_t& head, const elem_t& tail, const Roots& roots={} ) -> const Cell*
    {
        return new ( allocate( roots ) ) Cell( head, tail );
    }
};

typedef SimpleAllocator<SimpleScheme> Allocator;

}	//namespace

#endif
