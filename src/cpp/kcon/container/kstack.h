#ifndef HEADER_KCON_CONTAINER_KSTACK
#define HEADER_KCON_CONTAINER_KSTACK

namespace kcon { namespace container {

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
class basic_kstack
{
protected:
    typedef SchemeT<System> Scheme;
    typedef typename Scheme::pcell_t pcell_t;
    typedef typename Scheme::elem_t elem_t;
    typedef AllocatorT<System, SchemeT> Allocator;

    Allocator& _alloc;
    typename Allocator::template auto_root<elem_t> _items;

    basic_kstack( Allocator& alloc )
        : _alloc( alloc ), _items( _alloc ) {}

    void _push( elem_t item, const typename Allocator::Roots& roots )
    {
        _items = _alloc.new_Cell( item, _items, roots );
    }
public:
    void pop()
    {
        _items = _items.pcell()->tail();
    }

    bool empty() const { return _items.is_undef(); }
};

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT, class T>
struct kstack;

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
struct kstack<System, SchemeT, AllocatorT, typename SchemeT<System>::elem_t> : basic_kstack<System, SchemeT, AllocatorT>
{
    typedef typename SchemeT<System>::elem_t elem_t;
    typedef AllocatorT<System, SchemeT> Allocator;

    kstack( Allocator& alloc )
        : basic_kstack<System, SchemeT, AllocatorT>( alloc ) {}

    void push( elem_t item, const typename Allocator::Roots& roots={} )
    {
        this->_push( item, roots );
    }

    auto top() -> elem_t
    {
        return this->_items->head();
    }
};

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
struct kstack<System, SchemeT, AllocatorT, typename SchemeT<System>::pcell_t> : basic_kstack<System, SchemeT, AllocatorT>
{
    typedef typename SchemeT<System>::pcell_t pcell_t;
    typedef AllocatorT<System, SchemeT> Allocator;

    kstack( Allocator& alloc )
        : basic_kstack<System, SchemeT, AllocatorT>( alloc ) {}

    void push( pcell_t item, const typename Allocator::Roots& roots={} )
    {
        this->_push( item, roots );
    }

    auto top() -> pcell_t
    {
        return this->_items->head().pcell();
    }
};

} } //namespace

#endif // HEADER_KCON_KSTACK
