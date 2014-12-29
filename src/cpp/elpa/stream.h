#ifndef ELPA_STREAM_H
#define ELPA_STREAM_H

#include "types.h"
#include "memory.h"
#include "container/elpa_stack.h"
#include "container/elpa_map.h"
#include <ostream>
#include <istream>
#include <sstream>

namespace elpa {

struct Syntax;
struct EOS;
struct Undef;

template<class System, MetaScheme class SchemeT>
class elpa_ostream
{
    typedef SchemeT<System> Scheme;
    typedef typename Scheme::byte_t byte_t;
    typedef typename Scheme::pcell_t pcell_t;
    typedef typename Scheme::elem_t elem_t;

	std::ostream& _os;
	bool _flatten;

	auto _format( pcell_t pcell );
	auto _format( byte_t value );

    struct Tail { elem_t elem; size_t len; };
public:
	elpa_ostream( std::ostream& os, bool flatten=true )
		: _os( os ), _flatten( flatten ) {}

    auto setflatten( bool b ) -> elpa_ostream& { _flatten = b; return *this; }

	auto operator<<( pcell_t pcell ) -> elpa_ostream&;
	auto operator<<( byte_t value ) -> elpa_ostream&;
	auto operator<<( elem_t elem ) -> elpa_ostream&;

	template<class T>
    auto operator<<( const T& t ) -> elpa_ostream&
    {
        _os << t;
        return *this;
    }

    typedef elpa_ostream& (*ElpaManip)( elpa_ostream& );

    auto operator<<( ElpaManip m ) -> elpa_ostream&
    {
        return m(*this);
    }

    typedef std::ostream& (*Manip)( std::ostream& );

    auto operator<<( Manip m ) -> elpa_ostream&
    {
        m(_os);
        return *this;
    }
    
    typedef std::ios_base& (*BaseManip)( std::ios_base& );

    auto operator<<( BaseManip m ) -> elpa_ostream&
    {
        m(_os);
        return *this;
    }
};

template<class System, MetaScheme class SchemeT>
inline auto flat( elpa_ostream<System, SchemeT>& kos ) -> elpa_ostream<System, SchemeT>&
{
    return kos.setflatten( true );
}

template<class System, MetaScheme class SchemeT>
inline auto deep( elpa_ostream<System, SchemeT>& kos ) -> elpa_ostream<System, SchemeT>&
{
    return kos.setflatten( false );
}

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
class elpa_istream
{
    typedef SchemeT<System> Scheme;
    typedef typename Scheme::value_t value_t;
    typedef typename Scheme::byte_t byte_t;
    typedef typename Scheme::pcell_t pcell_t;
    typedef typename Scheme::elem_t elem_t;
    typedef AllocatorT<System,SchemeT> Allocator;

    template<class T>
    using auto_root = typename Allocator::template auto_root<T>;

    template<class T>
    using elpa_stack = elpa::container::elpa_stack<System, SchemeT, AllocatorT, T>;

	template<class K, class V>
    using elpa_map = elpa::container::elpa_map<System, SchemeT, AllocatorT, K, V>;
public:
	typedef elpa_map<std::string,elem_t> Defns;
private:
    std::istream& _is;
    Allocator& _alloc;
    const Defns& _defns;

	auto _parse_name() -> std::string;
    auto _parse_byte() -> byte_t;
	auto _parse_elems( std::vector< std::string >& names ) -> elem_t;
	auto _reverse_and_reduce( elem_t p, const std::vector< std::string >& names ) -> elem_t;
	auto _parse() -> elem_t;
public:
	elpa_istream( std::istream& is, Allocator& alloc, const Defns& defns )
        : _is( is ), _alloc( alloc ), _defns( defns ) {}

    elpa_istream( std::istream& is, Allocator& alloc )
        : _is( is ), _alloc( alloc ), _defns( Defns( alloc ) ) {}

	auto operator>>( elem_t& elem ) -> elpa_istream&;

	auto operator>>( char& c ) -> elpa_istream& { _is >> c; return *this; }
	auto operator>>( std::string& s ) -> elpa_istream& { _is >> s; return *this; }

    typedef elpa_istream& (*ElpaManip)( elpa_istream& );

    auto operator>>( ElpaManip m ) -> elpa_istream&
    {
        return m(*this);
    }

    typedef std::ios_base& (*Manip)( std::ios_base& );

    auto operator>>( Manip m ) -> elpa_istream&
    {
        _is >> m;
        return *this;
    }

    operator bool() const { return _is; }
    void putback( char c ) { _is.putback( c ); }
    bool get( char& c ) { return _is.get( c ); }
};

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
auto endofline( elpa_istream<System, SchemeT, AllocatorT>& eis ) -> auto&
{
    char c;
    while( eis.get(c) && (c != '\n') )
    	if( !isspace( c ) )
            throw Error<Syntax>( "Unexpected character after expression '"s + c + "'" );
    
    return eis;
}

}	//namespace

#endif