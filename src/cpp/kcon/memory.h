#ifndef HEADER_KCON_ALLOCATOR
#define HEADER_KCON_ALLOCATOR

#include "types.h"
#include <cstdint>
#include <initializer_list>
#include <set>
#include <vector>

namespace kcon {

struct OutOfMemory {};

#define MetaAllocator template<class System, template<class> class SchemeT>

/** Auto register roots with allocator instance **/

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
class AllocatorBase
{
protected:
	typedef SchemeT<System> Scheme;
	typedef AllocatorT<System, SchemeT> Allocator;
	typedef typename Scheme::elem_t elem_t;

	const size_t _ncells;
	size_t _gc_count;
	std::vector<elem_t*> _elem_roots;

	AllocatorBase( size_t ncells )
		: _ncells( ncells ), _gc_count( 0 )
    {
        System::assert( ncells > 0, "Allocator must allocate at least one cell" );
    }
public:
    auto gc_count() const -> size_t { return _gc_count; }

    void push_root( elem_t* root ) { _elem_roots.push_back( root ); }
	void pop_root( elem_t* root ) { System::assert( _elem_roots.back() == root, "Out of order root pop" ); _elem_roots.pop_back(); }

    template<class T>
    struct auto_root_ref : T
    {
        Allocator& alloc;

        auto_root_ref( Allocator& a, const T& root ) : T( root ), alloc( a ) {}
    };

    template<class T>
    class auto_root : public auto_root_ref<T>
    {
        explicit auto_root( const auto_root& );
        auto_root& operator=( const auto_root& );
    public:
        typedef auto_root_ref<T> ref;

        auto_root( Allocator& alloc, const T& root=T() ) : ref( alloc, root ) { this->alloc.push_root( this ); }
        auto_root( const auto_root_ref<T>& r ) : ref( r ) { this->alloc.push_root( this ); }
        ~auto_root() { this->alloc.pop_root( this ); }
        auto_root& operator=( const T& t ) { (T&) *this = t; return *this; }
    };
};

/**
    TestAllocator
    Simple but inefficient allocator for testing.
    It shifts all the cells on each memory allocation to test that the roots are correctly defined
**/

template<class System, MetaScheme class SchemeT>
struct TestAllocator : AllocatorBase< System, SchemeT, TestAllocator >
{
    typedef SchemeT<System> Scheme;
    typedef typename Scheme::value_t value_t;
    typedef typename Scheme::pcell_t pcell_t;
    typedef typename Scheme::elem_t elem_t;
    typedef typename Scheme::Cell Cell;
private:
    std::set<pcell_t> _allocated;

    static void _mark( std::set<pcell_t>& live, pcell_t pcell );
    void _shift();
public:
    TestAllocator( size_t ncells )
        : AllocatorBase<System, SchemeT, TestAllocator>( ncells ), _allocated() {}

	~TestAllocator()
	{
		for( auto p : _allocated )
            delete p;
	}

	void gc();

    auto new_Cell( const elem_t& head, const elem_t& tail ) -> const Cell*
    {
		elem_t e = System::check_address( new Cell( head, tail ) );	//switch pcell_t
		_allocated.insert( e.pcell() );
        this->push_root( &e );
        _shift();
        if( _allocated.size() == this->_ncells )
            gc();
        this->pop_root( &e );
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
struct SimpleAllocator : AllocatorBase<System, SchemeT, SimpleAllocator>
{
    typedef SchemeT<System> Scheme;
    typedef typename Scheme::value_t value_t;
    typedef typename Scheme::pcell_t pcell_t;
    typedef typename Scheme::elem_t elem_t;
    typedef typename Scheme::Cell Cell;
private:
    struct FreeCell
    {
        FreeCell* next;
        value_t _;
    };

    ASSERT( sizeof(FreeCell) == sizeof(Cell) );

    FreeCell* _page;
    FreeCell* _free_list;

    static void _mark( std::set<pcell_t>& live, pcell_t pcell );
public:
    SimpleAllocator( size_t ncells )
        : AllocatorBase<System, SchemeT, SimpleAllocator>( ncells ), _page( new FreeCell[ncells] ), _free_list( 0 )
    {
    	System::check_address( _page );
    	System::check_address( _page + ncells - 1 );
        System::assert( reinterpret_cast<uintptr_t>(_page) % sizeof(FreeCell) == 0, "Page not cell aligned" );
        gc();
        this->_gc_count = 0;
    }

	~SimpleAllocator()
	{
		delete[] _page;
	}

    void gc();

    auto num_allocated() const -> size_t
    {
        size_t n = this->_ncells;
        for( const FreeCell* p = _free_list; p != 0; p = p->next )
            --n;

        return n;
    }

    auto new_Cell( const elem_t& head, const elem_t& tail ) -> const Cell*
    {
        if( _free_list == 0 )   //remove
            gc();

        System::assert( _free_list != 0, "SimpleAllocator failed to reserve cell" );
        void* p = _free_list;
         _free_list = _free_list->next;

        elem_t e = new (p) Cell( head, tail );

        if( _free_list == 0 )
        {
            this->push_root( &e );
            gc();
            this->pop_root( &e );
        }

        return e.pcell();
    }
};

}	//namespace

#endif
