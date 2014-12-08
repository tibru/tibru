#include "stream.h"

std::ostream& KConOStream::operator<<( pcell_t pcell )
{
	_os << '[';
	_format( pcell );
	return _os << ']';
}

void KConOStream::visit( const Cell<pcell_t,pcell_t>* pcell )
{
	_os << '[';
	_format( pcell->head );
	_os << "] ";
	if( !_flatten ) _os << '[';
	_format( pcell->tail );
	if( !_flatten ) _os << ']';
}

void KConOStream::visit( const Cell<pcell_t,value_t>* pcell )
{
	_os << '[';
	_format( pcell->head );
	_os << "] " << pcell->tail;
}

void KConOStream::visit( const Cell<value_t,pcell_t>* pcell )
{
	_os << pcell->head << ' ';
	if( !_flatten ) _os << '[';
	_format( pcell->tail );
	if( !_flatten ) _os << ']';
}

void KConOStream::visit( const Cell<value_t,value_t>* pcell )
{
	_os << pcell->head << ' ' << pcell->tail;
}

void KConOStream::_format( pcell_t pcell )
{
	if( pcell.is_null() )
	{
		_os << "null";
		return;
	}

	pcell.dispatch( *this );
}
