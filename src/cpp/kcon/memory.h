#ifndef HEADER_KCON_ALLOCATOR
#define HEADER_KCON_ALLOCATOR

#include "types.h"
#include <cstdint>
#include <initializer_list>
#include <set>
#include <vector>

namespace kcon {

struct OutOfMemory {};

<<<<<<< HEAD
#define MetaAllocator template<class System, template<class> class SchemeTemplate>

/**
    TestAllocator
    Simple but inefficient allocator for testing.
    It shifts all the cells on each memory allocation to test that the roots are correctly defined
**/

template<class System, MetaScheme class SchemeT>
struct TestAllocator
=======
template<class Scheme>
struct SimpleAllocator
>>>>>>> 4edd44c6763ff556930f29ca9467a9495fdb8e21
{
    typedef SchemeT<System> Scheme;
    typedef typename Scheme::value_t value_t;
    typedef typename Scheme::pcell_t pcell_t;
    typedef typename Scheme::elem_t elem_t;
    typedef typename Scheme::Cell Cell;

<<<<<<< HEAD
    typedef std::vector<elem_t*> Roots;
private:
    const size_t _ncells;
    std::set<pcell_t> _allocated;
    size_t _gc_count;

    static void _mark( std::set<pcell_t>& live, pcell_t pcell );
    void _shift( const Roots& roots );
public:
    TestAllocator( size_t ncells )
        : _ncells( ncells ), _allocated(), _gc_count( 0 ) {}

	~TestAllocator()
	{
		for( auto p : _allocated )
            delete p;
	}

	void gc( const Roots& roots );

	auto gc_count() const -> size_t { return _gc_count; }

    auto new_Cell( const elem_t& head, const elem_t& tail, Roots roots ) -> const Cell*
    {
        if( _allocated.size() == _ncells )
            gc( roots );

        elem_t e = *_allocated.insert( new Cell( head, tail ) ).first;
        roots.push_back( &e );
        _shift( roots );
        return e.pcell();
    }

    auto num_allocated() const -> size_t
    {
        return _allocated.size();
    }
};

/**
    SimpleAllocator
    Simple semi-efficient allocator that uses a fixed page of cells
    Cells are never moved in memory
**/

template<class System, MetaScheme class SchemeT>
struct SimpleAllocator
{
    typedef SchemeT<System> Scheme;
    typedef typename Scheme::value_t value_t;
    typedef typename Scheme::pcell_t pcell_t;
    typedef typename Scheme::elem_t elem_t;
    typedef typename Scheme::Cell Cell;

    typedef std::vector<elem_t*> Roots;
private:
    struct FreeCell
    {
        FreeCell* next;
        value_t _;
    };

    ASSERT( sizeof(FreeCell) == sizeof(Cell) );

=======
    typedef std::initializer_list<pcell_t*> Roots;
private:
>>>>>>> 4edd44c6763ff556930f29ca9467a9495fdb8e21
    const size_t _ncells;
    std::vector<Cell> _page;    //effectively const to avoid reallocation
    std::set<Cell*> _free_set;
    size_t _gc_count;

<<<<<<< HEAD
    static void _mark( std::set<pcell_t>& live, pcell_t pcell );
=======
    auto _moved( elem_t e ) -> elem_t;
    auto _move( const Roots& roots ) -> void;
    static auto _mark( std::set<pcell_t>& live, pcell_t pcell ) -> void;

    SimpleAllocator( SimpleAllocator& );
    SimpleAllocator& operator=( const SimpleAllocator& );
>>>>>>> 4edd44c6763ff556930f29ca9467a9495fdb8e21
public:
    SimpleAllocator( size_t ncells )
        : _ncells( ncells ), _page( ncells, Cell( 1, 1 ) ), _free_set(), _gc_count( 0 )
    {
<<<<<<< HEAD
        System::assert( reinterpret_cast<uintptr_t>(_page) % sizeof(FreeCell) == 0, "Page not cell aligned" );
=======
>>>>>>> 4edd44c6763ff556930f29ca9467a9495fdb8e21
        gc({});
        _gc_count = 0;
    }

<<<<<<< HEAD
	~SimpleAllocator()
	{
		delete[] _page;
	}

    void gc( const Roots& roots );
=======
    auto gc( const Roots& roots ) -> void;
>>>>>>> 4edd44c6763ff556930f29ca9467a9495fdb8e21

    auto gc_count() const -> size_t { return _gc_count; }

    auto allocate( const Roots& roots ) -> void*
    {
        if( _free_set.empty() )
            gc( roots );

<<<<<<< HEAD
        void* p = _free_list;
        _free_list = _free_list->next;
=======
        void* p = *_free_set.begin();
        _free_set.erase( _free_set.begin() );
>>>>>>> 4edd44c6763ff556930f29ca9467a9495fdb8e21
        return p;
    }

    auto num_allocated() const -> size_t
    {
<<<<<<< HEAD
        size_t n = _ncells;
        for( const FreeCell* p = _free_list; p != 0; p = p->next )
            --n;

        return n;
=======
        return _ncells - _free_set.size();
>>>>>>> 4edd44c6763ff556930f29ca9467a9495fdb8e21
    }

    auto new_Cell( const elem_t& head, const elem_t& tail, const Roots& roots={} ) -> const Cell*
    {
        const Cell* p = new ( allocate( roots ) ) Cell( head, tail );
        _move( roots );
        return p;
    }
};

}	//namespace

#endif
