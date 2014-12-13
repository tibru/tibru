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
        _items = new (_alloc,roots) Cell<T,pcell_t>{ item, _items };
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
        if( item.is_cell() )
        {
            _pcell_items = new (_alloc,roots) Cell<pcell_t,pcell_t>{ item.pcell(), _pcell_items };
            _which_items = new (_alloc,roots) Cell<value_t,pcell_t>{ 0, _which_items };
        }
        else
        {
            _value_items = new (_alloc,roots) Cell<value_t,pcell_t>{ item.byte_value(), _value_items };
            _which_items = new (_alloc,roots) Cell<value_t,pcell_t>{ 1, _which_items };
        }
    }

    elem_t top()
    {
        value_t w = _which_items.cast<value_t,pcell_t>()->head;
        if( w == 0 )
            return _pcell_items.cast<pcell_t,pcell_t>()->head;
        else
            return _value_items.cast<value_t,pcell_t>()->head;
    }

    void pop()
    {
        value_t w = _which_items.cast<value_t,pcell_t>()->head;
        if( w == 0 )
            _pcell_items = _pcell_items.cast<pcell_t,pcell_t>()->tail;
        else
            _value_items = _value_items.cast<value_t,pcell_t>()->tail;

        _which_items = _which_items.cast<value_t,pcell_t>()->tail;
    }

    bool empty() const { return _pcell_items == pcell_t::null(); }
};

}   //namespace

#endif // HEADER_KCON_KSTACK
