#ifndef ELPA_STREAM_H
#define ELPA_STREAM_H

#include "types.h"
#include "memory.h"
#include "container/elpa_stack.h"
#include "container/elpa_map.h"
#include <ostream>
#include <istream>
#include <sstream>
#include <functional>

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

    void _format_elem( elem_t elem, const std::map<byte_t, std::string>& byte_names, const std::map<pcell_t, std::string>& cell_names );
	void _format( pcell_t pcell, const std::map<byte_t, std::string>& byte_names, const std::map<pcell_t, std::string>& cell_names );
	void _format( byte_t value, const std::map<byte_t, std::string>& byte_names );

    void _print_elem( elem_t elem, const std::map<byte_t, std::string>& byte_names, const std::map<pcell_t, std::string>& cell_names );
	void _print( pcell_t pcell, const std::map<byte_t, std::string>& byte_names, const std::map<pcell_t, std::string>& cell_names );
	void _print( byte_t value, const std::map<byte_t, std::string>& byte_names );

    struct Tail { elem_t elem; size_t len; };
public:
	elpa_ostream( std::ostream& os, bool flatten=true )
		: _os( os ), _flatten( flatten ) {}

    auto setflatten( bool b ) -> elpa_ostream& { _flatten = b; return *this; }

	auto operator<<( pcell_t pcell ) -> elpa_ostream& { _print( pcell, std::map<byte_t, std::string>(), std::map<pcell_t, std::string>() ); return *this; }
	auto operator<<( byte_t value ) -> elpa_ostream& { _print( value, std::map<byte_t, std::string>() ); return *this; }
	auto operator<<( elem_t elem ) -> elpa_ostream&
	{
	    std::map<byte_t, std::string> byte_names;
	    std::map<pcell_t, std::string> cell_names;

        _print_elem( elem, byte_names, cell_names );
        return *this;
	}

    template<class PMap>
	auto operator<<( const std::pair<PMap,elem_t> named_elem ) -> elpa_ostream&
	{
	    std::map<byte_t, std::string> byte_names;
	    std::map<pcell_t, std::string> cell_names;

	    for( auto named : *named_elem.first )
	    {
            if( named.second.is_byte() )
                byte_names[named.second.byte()] = named.first;
            else if( named.second.is_pcell() )
                cell_names[named.second.pcell()] = named.first;
	    }

	    elem_t elem = named_elem.second;

        _print_elem( elem, byte_names, cell_names );
        return *this;
	}

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
	typedef std::map<char, std::function< auto (Allocator&, std::istream&) -> elem_t> > Readers;
	typedef std::map<char, std::function< auto (Allocator&, elem_t, std::vector<std::string>&) -> elem_t> > Macros;
private:
    std::istream& _is;
    Allocator& _alloc;
    const Defns& _defns;
    const Readers& _readers;
    const Macros& _macros;

	auto _parse_name() -> std::string;
	auto _parse_reader( char c ) -> elem_t;
	auto _parse_macro( char c, elem_t tail, std::vector<std::string>& names ) -> elem_t;
    auto _parse_byte() -> byte_t;
	auto _parse_elems( std::vector< std::string >& names, size_t depth ) -> elem_t;
	auto _reverse_and_reduce( elem_t p, const std::vector< std::string >& names ) -> elem_t;
	auto _parse() -> elem_t;
public:
	elpa_istream( std::istream& is, Allocator& alloc, const Defns& defns, const Readers& readers, const Macros& macros )
        : _is( is ), _alloc( alloc ), _defns( defns ), _readers( readers ), _macros( macros ) {}

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

    operator bool() const { return bool(_is); }
    void putback( char c ) { _is.putback( c ); }
    bool get( char& c ) { return bool(_is.get( c )); }
};

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
auto endofline( elpa_istream<System, SchemeT, AllocatorT>& eis ) -> auto&
{
    char c;
    while( eis.get(c) && (c != '\n') )
    	if( !isspace( c ) )
            throw Error<Syntax>( "Unexpected character '"s + c + "' after expression" );

    return eis;
}

}	//namespace

#endif
