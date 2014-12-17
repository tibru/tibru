#ifndef HEADER_KCON_CONTAIMER_KSTACK
#define HEADER_KCON_CONTAIMER_KSTACK

namespace kcon { namespace container {

template<class Scheme>
class basic_kstack
{
protected:
    typedef typename Scheme::pcell_t pcell_t;
    typedef typename Scheme::elem_t elem_t;

    Allocator& _alloc;
    pcell_t _items;

    basic_kstack( Allocator& alloc )
        : _alloc( alloc ), _items( null<pcell_t>() ) {}
public:
    pcell_t& items() { return _items; }

    void push( elem_t item, Allocator::Roots roots )
    {
        _items = _alloc.new_Cell( item, _items, roots );
    }

    void pop()
    {
        _items = _items->tail().pcell();
    }

    bool empty() const { return _items == null<pcell_t>(); }
};

template<class Scheme, class T>
struct kstack;

template<class Scheme>
struct kstack<Scheme,typename Scheme::elem_t> : basic_kstack<Scheme>
{
    typedef typename Scheme::elem_t elem_t;

    kstack( Allocator& alloc )
        : basic_kstack<Scheme>( alloc ) {}

    auto top() -> elem_t
    {
        return this->_items->head();
    }
};

template<class Scheme>
struct kstack<Scheme,typename Scheme::pcell_t> : basic_kstack<Scheme>
{
    typedef typename Scheme::pcell_t pcell_t;

    kstack( Allocator& alloc )
        : basic_kstack<Scheme>( alloc ) {}

    auto top() -> pcell_t
    {
        return this->_items->head().pcell();
    }
};

} } //namespace

#endif // HEADER_KCON_KSTACK
