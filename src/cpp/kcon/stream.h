#ifndef KCON_STREAM_H
#define KCON_STREAM_H

#include <ostream>
#include "types.h"

class kostream
{
	std::ostream& _os;
	bool _flatten;

	void _format( const Cell<pcell_t,pcell_t>* pcell );
	void _format( const Cell<pcell_t,value_t>* pcell );
	void _format( const Cell<value_t,pcell_t>* pcell );
	void _format( const Cell<value_t,value_t>* pcell );
	void _format( pcell_t pcell );
	void _format( value_t value );
public:
	kostream( std::ostream& os, bool flatten=true )
		: _os( os ), _flatten( flatten ) {}

    kostream& setflatten( bool b ) { _flatten = b; return *this; }

	std::ostream& operator<<( pcell_t pcell );
	std::ostream& operator<<( value_t value );
	std::ostream& operator<<( elem_t elem );

    template<class T>
    kostream& operator<<( const T& t )
    {
        _os << t;
        return *this;
    }

    typedef kostream& (*Manip)( kostream& );

    kostream& operator<<( Manip m )
    {
        return m(*this);
    }
};

inline kostream& flat( kostream& kos )
{
    return kos.setflatten( true );
}

inline kostream& deep( kostream& kos )
{
    return kos.setflatten( false );
}

#endif
