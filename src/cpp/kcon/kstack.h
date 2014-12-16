#ifndef HEADER_KCON_KSTACK
#define HEADER_KCON_KSTACK

namespace kcon {

template<class Scheme>
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
        _items = _alloc.new_Cell( item, _items, roots );
    }

    void pop()
    {
        _items = _items->tail().pcell();
    }

    bool empty() const { return _items == null<pcell_t>(); }
};

template<class Scheme, class T>
struct _kstack;

template<class Scheme>
struct _kstack<Scheme,elem_t> : basic_kstack<Scheme>
{
    _kstack( Allocator& alloc )
        : basic_kstack<Scheme>( alloc ) {}

    elem_t top()
    {
        return this->_items->head();
    }
};

template<class Scheme>
struct _kstack<Scheme,pcell_t> : basic_kstack<Scheme>
{
    _kstack( Allocator& alloc )
        : basic_kstack<Scheme>( alloc ) {}

    pcell_t top()
    {
        return this->_items->head().pcell();
    }
};

template<class T>
using kstack = _kstack<SimpleScheme,T>;

}   //namespace

#endif // HEADER_KCON_KSTACK
