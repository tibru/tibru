#include "Printer.h"

std::ostream& Printer::operator<<( pcell_t pcell )
{
	_os << '[';
	_print( pcell );
	return _os << ']';
}

void Printer::_print( const Node<pcell_t,pcell_t>* pcell )
{
	_os << '[';
	_print( pcell->head );
	_os << "] ";
	if( !_flatten ) _os << '[';
	_print( pcell->tail );
	if( !_flatten ) _os << ']';
}

void Printer::_print( const Node<pcell_t,value_t>* pcell )
{
	_os << '[';
	_print( pcell->head );
	_os << "] " << pcell->tail;
}

void Printer::_print( const Node<value_t,pcell_t>* pcell )
{
	_os << pcell->head << ' ';
	if( !_flatten ) _os << '[';
	_print( pcell->tail );
	if( !_flatten ) _os << ']';
}

void Printer::_print( const Node<value_t,value_t>* pcell )
{
	_os << pcell->head << ' ' << pcell->tail;
}

void Printer::_print( pcell_t pcell )
{
	if( pcell.is_null() )
	{
		_os << "null";
		return;
	}

	switch( pcell.typecode() )
	{
		case Node<pcell_t,pcell_t>::TYPECODE:
			_print( pcell.cast<pcell_t,pcell_t>() );
			break;
		case Node<pcell_t,value_t>::TYPECODE:
			_print( pcell.cast<pcell_t,value_t>() );
			break;
		case Node<value_t,pcell_t>::TYPECODE:
			_print( pcell.cast<value_t,pcell_t>() );
			break;
		case Node<value_t,value_t>::TYPECODE:
			_print( pcell.cast<value_t,value_t>() );
			break;
		default:
			error( "<< dispatch failed" );
	}
}
