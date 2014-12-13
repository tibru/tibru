#ifndef HEADER_KCON_KSTACK
#define HEADER_KCON_KSTACK

namespace kcon {

template<class T>
class kstack
{
    Allocator& _alloc;
    pcell_t _items;
public:
    kstack( Allocator& alloc )
        : _alloc( alloc ), _items( pcell_t::null() ) {}

    void push( const T& item )
    {
        _items = new (_alloc,{&_items}) Cell<T,pcell_t>{ item, _items };
    }

    const T& top()
    {
        return _items.cast<T,pcell_t>()->head;
    }

    void pop()
    {
        _items = _items.cast<T,pcell_t>()->tail;
    }

    bool empty() const { return _items == pcell_t::null(); }
};

}   //namespace

#endif // HEADER_KCON_KSTACK
