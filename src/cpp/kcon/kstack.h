#ifndef HEADER_KCON_KSTACK
#define HEADER_KCON_KSTACK

namespace kcon {

class basic_kstack
{
protected:
    Allocator& _alloc;
    pcell_t _items;

    basic_kstack( Allocator& alloc )
        : _alloc( alloc ), _items( null<pcell_t>() ) {}
public:
    pcell_t& items() { return _items; }

    void push( elem_t item, Allocator::Roots roots )
    {
        _items = new (_alloc,roots) Cell{ item, _items };
    }

    void pop()
    {
        _items = _items->tail().pcell();
    }

    bool empty() const { return _items == null<pcell_t>(); }
};

template<class T>
struct kstack;

template<>
struct kstack<elem_t> : basic_kstack
{
    kstack( Allocator& alloc )
        : basic_kstack( alloc ) {}

    elem_t top()
    {
        return _items->head();
    }
};

template<>
struct kstack<pcell_t> : basic_kstack
{
    kstack( Allocator& alloc )
        : basic_kstack( alloc ) {}

    pcell_t top()
    {
        return _items->head().pcell();
    }
};

}   //namespace

#endif // HEADER_KCON_KSTACK
