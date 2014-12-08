#ifndef KCON_STREAM_H
#define KCON_STREAM_H

#include <ostream>
#include "types.h"

class KConOStream : public CellVisitor
{
	std::ostream& _os;
	bool _flatten;

	void _format( pcell_t pcell );
public:
	KConOStream( std::ostream& os, bool flatten=true )
		: _os( os ), _flatten( flatten ) {}

    KConOStream& setflatten( bool b ) { _flatten = b; return *this; }

	void visit( const Cell<pcell_t,pcell_t>* pcell );
	void visit( const Cell<pcell_t,value_t>* pcell );
	void visit( const Cell<value_t,pcell_t>* pcell );
	void visit( const Cell<value_t,value_t>* pcell );

	std::ostream& operator<<( pcell_t pcell );

    template<class T>
    KConOStream& operator<<( const T& t )
    {
        _os << t;
        return *this;
    }

    typedef KConOStream& (*Manip)( KConOStream& );

    KConOStream& operator<<( Manip m )
    {
        return m(*this);
    }
};

inline KConOStream& flat( KConOStream& kos )
{
    return kos.setflatten( true );
}

inline KConOStream& deep( KConOStream& kos )
{
    return kos.setflatten( false );
}

#endif
