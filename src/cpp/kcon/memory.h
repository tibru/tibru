#ifndef HEADER_KCON_ALLOCATOR
#define HEADER_KCON_ALLOCATOR

#include "types.h"
#include <cstdint>
#include <cstdlib>
#include <initializer_list>
#include <set>

namespace kcon {

struct OutOfMemory {};

template<class Scheme>
class TestAllocator
{
    typedef typename Scheme::value_t value_t;
    typedef typename Scheme::pcell_t pcell_t;
    typedef typename Scheme::elem_t elem_t;
    typedef typename Scheme::Cell Cell;

    const size_t _ncells;
    std::set<pcell_t> _allocated;
    size_t _gc_count;

    static void _mark( std::set<pcell_t>& live, pcell_t pcell );
public:
    typedef std::initializer_list<pcell_t*> Roots;

    TestAllocator( size_t ncells )
        : _ncells( ncells ), _allocated(), _gc_count( 0 ) {}

	~TestAllocator()
	{
		for( auto p : _allocated )
            delete p;
	}

	void gc( const Roots& roots );

	auto gc_count() const -> size_t { return _gc_count; }

    auto new_Cell( const elem_t& head, const elem_t& tail, const Roots& roots={} ) -> const Cell*
    {
        if( _allocated.size() == _ncells )
            gc( roots );

        return *_allocated.insert( new Cell( head, tail ) ).first;
    }

    auto num_allocated() const -> size_t
    {
        return _allocated.size();
    }
};

template<class Scheme>
class SimpleAllocator
{
    typedef typename Scheme::value_t value_t;
    typedef typename Scheme::pcell_t pcell_t;
    typedef typename Scheme::elem_t elem_t;
    typedef typename Scheme::Cell Cell;

    class FreeCell
    {
        value_t _salt;
        FreeCell* _next;

        static auto _hash( value_t salt, FreeCell* p ) -> auto
        {
            return reinterpret_cast<FreeCell*>( reinterpret_cast<uintptr_t>( p ) ^ salt );
        }
    public:
        FreeCell( FreeCell* next=0 )
            : _salt( rand() & ADDR_MASK ), _next( _hash( _salt, next ) ) {}

        auto next() const -> FreeCell* { return _hash( _salt, _next ); }
    };

    ASSERT( sizeof(FreeCell) == sizeof(Cell) );

    const size_t _ncells;
    FreeCell* _page;
    FreeCell* _free_list;
    size_t _gc_count;

    static void _mark( std::set<pcell_t>& live, pcell_t pcell );
public:
    typedef std::initializer_list<pcell_t*> Roots;

    SimpleAllocator( size_t ncells )
        : _ncells( ncells ), _page( new FreeCell[ncells] ), _free_list( 0 ), _gc_count( 0 )
    {
        assert( reinterpret_cast<uintptr_t>(_page) % sizeof(FreeCell) == 0, "Page not cell aligned" );
        gc({});
        _gc_count = 0;
    }

	~SimpleAllocator()
	{
		delete[] _page;
	}

    void gc( const Roots& roots );

    auto gc_count() const -> size_t { return _gc_count; }

    auto allocate( const Roots& roots ) -> void*
    {
        if( _free_list == 0 )
            gc( roots );

        void* p = _free_list;
        _free_list = _free_list->next();
        return p;
    }

    auto num_allocated() const -> size_t
    {
        size_t n = _ncells;
        for( const FreeCell* p = _free_list; p != 0; p = p->next() )
            --n;

        return n;
    }

    auto new_Cell( const elem_t& head, const elem_t& tail, const Roots& roots={} ) -> const Cell*
    {
        return new ( allocate( roots ) ) Cell( head, tail );
    }
};

}	//namespace

#endif
