#ifndef HEADER_ELPA_CONTAINER_ELPA_STACK
#define HEADER_ELPA_CONTAINER_ELPA_STACK

#include "../types.h"
#include "../memory.h"

namespace elpa { namespace container {

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
class basic_elpa_stack
{
protected:
    typedef SchemeT<System> Scheme;
    typedef typename Scheme::pcell_t pcell_t;
    typedef typename Scheme::elem_t elem_t;
    typedef AllocatorT<System, SchemeT> Allocator;

    Allocator& _alloc;
    typename Allocator::template auto_root<elem_t> _items;

    basic_elpa_stack( Allocator& alloc )
        : _alloc( alloc ), _items( _alloc ) {}

    void _push( elem_t item )
    {
        _items = _alloc.new_Cell( item, _items );
    }
public:
    void pop()
    {
        _items = _items.pcell()->tail();
    }

    bool empty() const { return _items.is_undef(); }

    elem_t items() const { return _items; }
};

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT, class T>
struct elpa_stack;

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
struct elpa_stack<System, SchemeT, AllocatorT, typename SchemeT<System>::elem_t> : basic_elpa_stack<System, SchemeT, AllocatorT>
{
    typedef typename SchemeT<System>::elem_t elem_t;
    typedef AllocatorT<System, SchemeT> Allocator;

    elpa_stack( Allocator& alloc )
        : basic_elpa_stack<System, SchemeT, AllocatorT>( alloc ) {}

    void push( elem_t item )
    {
        this->_push( item );
    }

    auto top() -> elem_t
    {
        return this->_items->head();
    }
};

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
struct elpa_stack<System, SchemeT, AllocatorT, typename SchemeT<System>::pcell_t> : basic_elpa_stack<System, SchemeT, AllocatorT>
{
    typedef typename SchemeT<System>::pcell_t pcell_t;
    typedef AllocatorT<System, SchemeT> Allocator;

    elpa_stack( Allocator& alloc )
        : basic_elpa_stack<System, SchemeT, AllocatorT>( alloc ) {}

    void push( pcell_t item )
    {
        this->_push( item );
    }

    auto top() -> pcell_t
    {
        return this->_items->head().pcell();
    }
};

} } //namespace

#endif // HEADER_ELPA_KSTACK
