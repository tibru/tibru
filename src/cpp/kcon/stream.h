#ifndef KCON_STREAM_H
#define KCON_STREAM_H

#include "types.h"
#include "memory.h"
#include <ostream>
#include <istream>
#include <sstream>

namespace kcon {

struct Syntax;
struct EOS;

template<class Scheme>
class _kostream
{
	std::ostream& _os;
	bool _flatten;

	void _format( pcell_t pcell );
	void _format( byte_t value );
public:
	_kostream( std::ostream& os, bool flatten=true )
		: _os( os ), _flatten( flatten ) {}

    _kostream& setflatten( bool b ) { _flatten = b; return *this; }

	_kostream& operator<<( pcell_t pcell );
	_kostream& operator<<( byte_t value );
	_kostream& operator<<( elem_t elem );

    template<class T>
    _kostream& operator<<( const T& t )
    {
        _os << t;
        return *this;
    }

    typedef _kostream& (*KManip)( _kostream& );

    _kostream& operator<<( KManip m )
    {
        return m(*this);
    }

    typedef std::ostream& (*Manip)( std::ostream& );

    _kostream& operator<<( Manip m )
    {
        m(_os);
        return *this;
    }
};

template<class Scheme>
inline _kostream<Scheme>& flat( _kostream<Scheme>& kos )
{
    return kos.setflatten( true );
}

template<class Scheme>
inline _kostream<Scheme>& deep( _kostream<Scheme>& kos )
{
    return kos.setflatten( false );
}

class kistream
{
    std::istream& _is;
    Allocator& _alloc;

    byte_t _parse_byte();
	pcell_t _parse_elems();
	pcell_t _reverse_and_reduce( pcell_t p );
	elem_t _parse();
public:
    kistream( std::istream& is, Allocator& alloc )
        : _is( is ), _alloc( alloc ) {}

	kistream& operator>>( elem_t& elem );
};

template<class Alloc>
elem_t parse( Alloc& allocator, const std::string& in )
{
    std::istringstream iss( in );
    elem_t elem = null<elem_t>();
    kistream( iss, allocator ) >> elem;
    return elem;
}

}	//namespace

#endif
