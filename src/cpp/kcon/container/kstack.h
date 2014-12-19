#ifndef HEADER_KCON_CONTAIMER_KSTACK
#define HEADER_KCON_CONTAIMER_KSTACK

namespace kcon { namespace container {

template<class Scheme, class Allocator>
class basic_kstack
{
protected:
    typedef typename Scheme::pcell_t pcell_t;
    typedef typename Scheme::elem_t elem_t;

    Allocator& _alloc;
    elem_t _items;

    basic_kstack( Allocator& alloc )
        : _alloc( alloc ), _items() {}

    void _push( elem_t item, const typename Allocator::Roots& roots )
    {
        _items = _alloc.new_Cell( item, _items, roots );
    }
public:
    elem_t& items() { return _items; }

    void pop()
    {
        _items = _items.pcell()->tail();
    }

    bool empty() const { return _items.is_undef(); }
};

template<class Scheme, class Allocator, class T>
struct kstack;

template<class Scheme, class Allocator>
struct kstack<Scheme, Allocator, typename Scheme::elem_t> : basic_kstack<Scheme,Allocator>
{
    typedef typename Scheme::elem_t elem_t;

    kstack( Allocator& alloc )
        : basic_kstack<Scheme, Allocator>( alloc ) {}

    void push( elem_t item, const typename Allocator::Roots& roots )
    {
        this->_push( item, roots );
    }

    auto top() -> elem_t
    {
        return this->_items->head();
    }
};

template<class Scheme, class Allocator>
struct kstack<Scheme, Allocator, typename Scheme::pcell_t> : basic_kstack<Scheme,Allocator>
{
    typedef typename Scheme::pcell_t pcell_t;

    kstack( Allocator& alloc )
        : basic_kstack<Scheme, Allocator>( alloc ) {}

    void push( pcell_t item, const typename Allocator::Roots& roots )
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
