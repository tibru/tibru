#ifndef HEADER_ELPA_ALLOCATOR
#define HEADER_ELPA_ALLOCATOR

#include "types.h"
#include <cstdint>
#include <initializer_list>
#include <set>

namespace elpa {

struct OutOfMemory {};

#define MetaAllocator template<class System, template<class> class SchemeT>

/** Auto register roots with allocator instance **/

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
class AllocatorBase
{
protected:
	typedef SchemeT<System> Scheme;
	typedef AllocatorT<System, SchemeT> Allocator;
	typedef typename Scheme::pcell_t pcell_t;
	typedef typename Scheme::elem_t elem_t;

	const size_t _ncells;
	size_t _gc_count;
	std::set<elem_t*> _elem_roots;
	std::set<pcell_t*> _pcell_roots;

	AllocatorBase( size_t ncells )
		: _ncells( ncells ), _gc_count( 0 )
    {
        System::assert( ncells > 0, "Allocator must allocate at least one cell" );
    }
public:
    auto gc_count() const -> size_t { return _gc_count; }

    auto num_max() const -> size_t { return _ncells; }

    void add_root( elem_t* root ) { _elem_roots.insert( root ); }
	void del_root( elem_t* root ) { _elem_roots.erase( root ); }
    void add_root( pcell_t* root ) { _pcell_roots.insert( root ); }
	void del_root( pcell_t* root ) { _pcell_roots.erase( root ); }

    template<class T>
    struct auto_root_ref : T
    {
        Allocator& alloc;

        auto_root_ref( Allocator& a, const T& root ) : T( root ), alloc( a ) {}

        auto_root_ref& operator=( const auto_root_ref& ar )
        {
            System::assert( &this->alloc == &ar.alloc, "Mismatch of allocator in auto_root_ref assignment" );
            T::operator=( ar );
            return *this;
        }

        T* addr() { return this; }
    };

    template<class T>
    class auto_root : public auto_root_ref<T>
    {
        explicit auto_root( const auto_root& );
    public:
        typedef auto_root_ref<T> ref;

        auto_root& operator=( const auto_root& ar )
        {
            ref::operator=( ar );
            return *this;
        }

        auto_root( Allocator& alloc, const T& root=T() ) : ref( alloc, root ) { this->alloc.add_root( this->addr() ); }
        auto_root( const auto_root_ref<T>& r ) : ref( r ) { this->alloc.add_root( this->addr() ); }
        ~auto_root() { this->alloc.del_root( this->addr() ); }
        auto_root& operator=( const T& t ) { (T&) *this = t; return *this; }
    };
};

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
template<class T>
struct AllocatorBase<System, SchemeT, AllocatorT>::auto_root_ref<T*>
{
    typedef AllocatorT<System,SchemeT> Allocator;

    T* ptr;
    Allocator& alloc;

    auto_root_ref( Allocator& a, T* root ) : ptr( root ), alloc( a ) {}

    T* operator->() const { return ptr; }
    operator T*() const { return ptr; }

    T** addr() { return &ptr; }
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
	static auto name() -> std::string { return "test"; }

    TestAllocator( size_t ncells )
        : AllocatorBase<System, SchemeT, TestAllocator>( ncells ), _allocated() {}

	~TestAllocator()
	{
		for( auto p : _allocated )
            delete p;
	}

	void gc();

    auto new_Cell( const elem_t& head, const elem_t& tail ) -> pcell_t
    {
		pcell_t p = System::check_address( new Cell( head, tail ) );
		_allocated.insert( p );
        this->add_root( &p );
        _shift();
        if( _allocated.size() == this->_ncells )
            gc();
        this->del_root( &p );
        return p;
    }

    auto num_allocated() const -> size_t
    {
        return _allocated.size();
    }

    auto num_total() const -> size_t
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
        elem_t _;
    };

    ASSERT( sizeof(FreeCell) == sizeof(Cell) );

    FreeCell* _page;
    FreeCell* _free_list;

    auto _next() -> void*
    {
        System::assert( _free_list != 0, "SimpleAllocator failed to reserve cell" );

        void* p = _free_list;
        _free_list = _free_list->next;
        return p;
    }

    static void _mark( std::set<pcell_t>& live, pcell_t pcell );
public:
	static auto name() -> std::string { return "simple"; }

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

    auto num_total() const -> size_t
    {
    	return this->_ncells;
    }

    auto new_Cell( const elem_t& head, const elem_t& tail ) -> pcell_t
    {
        pcell_t p = new (_next()) Cell( head, tail );

        if( _free_list == 0 )
        {
            this->add_root( &p );
            gc();
            this->del_root( &p );
        }

        return p;
    }
};

template<class System, MetaScheme class SchemeT>
using OptAllocator = SimpleAllocator<System, SchemeT>;

}	//namespace

#endif
