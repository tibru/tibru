#ifndef HEADER_KCON_ALLOCATOR
#define HEADER_KCON_ALLOCATOR

#include "types.h"
#include <cstdint>
#include <initializer_list>
#include <set>
#include <vector>

namespace kcon {

struct OutOfMemory {};

#define MetaAllocator template<class System, template<class> class SchemeTemplate>

/** Auto register roots with allocator instance **/

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT, class T>
class auto_root : public T
{
    typedef AllocatorT<System,SchemeT> Allocator;

    Allocator& _alloc;

    auto_root( const auto_root& );
    auto_root& operator=( const auto_root& );
public:
    auto_root( Allocator& alloc, const T& root=T() )
        : T( root ), _alloc( alloc )
    {
        _alloc.push_root( this );
    }

    ~auto_root()
    {
        _alloc.pop_root( this );
    }

    auto_root& operator=( const T& t )
    {
        (T&) *this = t;
        return *this;
    }
};

/**
    TestAllocator
    Simple but inefficient allocator for testing.
    It shifts all the cells on each memory allocation to test that the roots are correctly defined
**/

template<class System, MetaScheme class SchemeT>
struct TestAllocator
{
    typedef SchemeT<System> Scheme;
    typedef typename Scheme::value_t value_t;
    typedef typename Scheme::pcell_t pcell_t;
    typedef typename Scheme::elem_t elem_t;
    typedef typename Scheme::Cell Cell;

    template<class T>
    using auto_root = auto_root< System, SchemeT, TestAllocator, T>;

    typedef std::vector<elem_t*> Roots;
private:
    const size_t _ncells;
    std::set<pcell_t> _allocated;
    std::vector<elem_t*> _elem_roots;
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

	void push_root( elem_t* root ) { _elem_roots.push_back( root ); }
	void pop_root( elem_t* root ) { System::assert( _elem_roots.back() == root, "Out of order root pop" ); _elem_roots.pop_back(); }

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

    template<class T>
    using auto_root = auto_root< System, SchemeT, SimpleAllocator, T>;

    typedef std::vector<elem_t*> Roots;
private:
    struct FreeCell
    {
        FreeCell* next;
        value_t _;
    };

    ASSERT( sizeof(FreeCell) == sizeof(Cell) );

    const size_t _ncells;
    FreeCell* _page;
    FreeCell* _free_list;
    std::vector<elem_t*> _elem_roots;
    size_t _gc_count;

    static void _mark( std::set<pcell_t>& live, pcell_t pcell );
public:
    SimpleAllocator( size_t ncells )
        : _ncells( ncells ), _page( new FreeCell[ncells] ), _free_list( 0 ), _gc_count( 0 )
    {
        System::assert( reinterpret_cast<uintptr_t>(_page) % sizeof(FreeCell) == 0, "Page not cell aligned" );
        gc({});
        _gc_count = 0;
    }

	~SimpleAllocator()
	{
		delete[] _page;
	}

	void push_root( elem_t* root ) { _elem_roots.push_back( root ); }
	void pop_root( elem_t* root ) { System::assert( _elem_roots.back() == root, "Out of order root pop" ); _elem_roots.pop_back(); }

    void gc( const Roots& roots );

    auto gc_count() const -> size_t { return _gc_count; }

    auto allocate( const Roots& roots ) -> void*
    {
        if( _free_list == 0 )
            gc( roots );

        void* p = _free_list;
        _free_list = _free_list->next;
        return p;
    }

    auto num_allocated() const -> size_t
    {
        size_t n = _ncells;
        for( const FreeCell* p = _free_list; p != 0; p = p->next )
            --n;

        return n;
    }

    auto new_Cell( const elem_t& head, const elem_t& tail, const Roots& roots={} ) -> const Cell*
    {
        //assert 1 free
        return new ( allocate( roots ) ) Cell( head, tail );
        //reserve 1 roots + p
    }
};

}	//namespace

#endif
