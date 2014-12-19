#ifndef KCON_STREAM_H
#define KCON_STREAM_H

#include "types.h"
#include "memory.h"
#include "container/kstack.h"   //remove
#include <ostream>
#include <istream>
#include <sstream>

namespace kcon {

struct Syntax;
struct EOS;

template<class Scheme>
class kostream
{
    typedef typename Scheme::byte_t byte_t;
    typedef typename Scheme::pcell_t pcell_t;
    typedef typename Scheme::elem_t elem_t;

	std::ostream& _os;
	bool _flatten;

	auto _format( pcell_t pcell );
	auto _format( byte_t value );

    struct Tail { elem_t elem; size_t len; };
public:
	kostream( std::ostream& os, bool flatten=true )
		: _os( os ), _flatten( flatten ) {}

    auto setflatten( bool b ) -> kostream& { _flatten = b; return *this; }

	auto operator<<( pcell_t pcell ) -> kostream&;
	auto operator<<( byte_t value ) -> kostream&;
	auto operator<<( elem_t elem ) -> kostream&;

    template<class T>
    auto operator<<( const T& t ) -> kostream&
    {
        _os << t;
        return *this;
    }

    typedef kostream& (*KManip)( kostream& );

    auto operator<<( KManip m ) -> kostream&
    {
        return m(*this);
    }

    typedef std::ostream& (*Manip)( std::ostream& );

    auto operator<<( Manip m ) -> kostream&
    {
        m(_os);
        return *this;
    }
};

template<class Scheme>
inline kostream<Scheme>& flat( kostream<Scheme>& kos )
{
    return kos.setflatten( true );
}

template<class Scheme>
inline kostream<Scheme>& deep( kostream<Scheme>& kos )
{
    return kos.setflatten( false );
}

template<class Scheme, class Allocator>
class kistream
{
    typedef typename Scheme::value_t value_t;
    typedef typename Scheme::byte_t byte_t;
    typedef typename Scheme::pcell_t pcell_t;
    typedef typename Scheme::elem_t elem_t;

    template<class T>
    using kstack = kcon::container::kstack<Scheme,Allocator,T>; //remove

    std::istream& _is;
    Allocator& _alloc;
    std::vector<elem_t*> _roots;

    auto _parse_byte() -> byte_t;
	auto _parse_elems() -> elem_t;
	auto _reverse_and_reduce( elem_t p ) -> elem_t;
	auto _parse() -> elem_t;
public:
    kistream( std::istream& is, Allocator& alloc, const std::vector<elem_t*>& roots )
        : _is( is ), _alloc( alloc ), _roots( roots ) {}

	auto operator>>( elem_t& elem ) -> kistream&;
};

}	//namespace

#endif
