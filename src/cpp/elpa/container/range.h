#ifndef HEADER_ELPA_CONTAINER_RANGE
#define HEADER_ELPA_CONTAINER_RANGE

namespace elpa { namespace container {

template<class T>
struct basic_valrange
{
    class const_iterator
    {
        T _value;
    public:
        const_iterator( const T& n ) : _value( n ) {}
        const T& operator*() const { return _value; }
        bool operator!=( const_iterator i ) const { return _value != i._value; }
        const_iterator operator++() { _value++; return *this; }
    };
private:
    const const_iterator _begin;
    const const_iterator _end;
public:
    basic_valrange( const T& n ) : _begin(), _end( n ) {}
    basic_valrange( const T& b, const T& e ) : _begin( b ), _end( e ) {}

    const_iterator begin() const { return _begin; }
    const_iterator end() const { return _end; }
};

template<class T>
inline auto valrange( const T& begin, const T& end ) -> basic_valrange<T>
{
    return basic_valrange<T>( begin, end );
}

template<class T>
inline auto valrange( const T& end ) -> basic_valrange<T>
{
    return basic_valrange<T>( end );
}


template<class Iter>
struct basic_range
{
    typedef Iter const_iterator;
private:
    const const_iterator _begin;
    const const_iterator _end;
public:
    basic_range( const Iter& b, const Iter& e ) : _begin( b ), _end( e ) {}

    const_iterator begin() const { return _begin; }
    const_iterator end() const { return _end; }
};

template<class Iter>
inline auto range( const Iter& b, const Iter& e ) -> basic_range<Iter>
{
    return basic_range<Iter>( b, e );
}

} } //namespace

#endif
