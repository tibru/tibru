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

    pcell_t& items() { return _items; }

    void push( const T& item, Allocator::Roots roots )
    {
        _items = new (_alloc,roots) Cell{ item, _items };
    }

    const T& top()
    {
        return _items->head.pcell;
    }

    void pop()
    {
        _items = _items->tail.pcell;
    }

    bool empty() const { return _items == pcell_t::null(); }
};

template<>
class kstack<elem_t>
{
    Allocator& _alloc;
    pcell_t _pcell_items;
    pcell_t _value_items;
    pcell_t _which_items;
public:
    kstack( Allocator& alloc )
        : _alloc( alloc ), _pcell_items( pcell_t::null() ), _value_items( pcell_t::null() ), _which_items( pcell_t::null() ) {}

    pcell_t& items() { return _pcell_items; }

    void push( const elem_t& item, Allocator::Roots roots )
    {
        if( item.is_pcell() )
        {
            _pcell_items = new (_alloc,roots) Cell{ item.pcell, _pcell_items };
            _which_items = new (_alloc,roots) Cell{ byte_t(0), _which_items };
        }
        else
        {
            _value_items = new (_alloc,roots) Cell{ item.value, _value_items };
            _which_items = new (_alloc,roots) Cell{ 1, _which_items };
        }
    }

    elem_t top()
    {
        value_t w = _which_items->head.value;
        if( w == 0 )
            return _pcell_items->head;
        else
            return _value_items->head;
    }

    void pop()
    {
        value_t w = _which_items->head.value;
        if( w == 0 )
            _pcell_items = _pcell_items->tail.pcell;
        else
            _value_items = _value_items->tail.pcell;

        _which_items = _which_items->tail.pcell;
    }

    bool empty() const { return _pcell_items == pcell_t::null(); }
};

}   //namespace

#endif // HEADER_KCON_KSTACK
