#include "Printer.h"

std::ostream& Printer::operator<<( pnode_t pnode )
{
	_os << '[';
	_print( pnode );
	return _os << ']';
}

void Printer::_print( const Node<pnode_t,pnode_t>* pnode )
{
	_os << '[';
	_print( pnode->head );
	_os << "] ";
	if( !_flatten ) _os << '[';
	_print( pnode->tail );
	if( !_flatten ) _os << ']';
}

void Printer::_print( const Node<pnode_t,value_t>* pnode )
{
	_os << '[';
	_print( pnode->head );
	_os << "] " << pnode->tail;
}

void Printer::_print( const Node<value_t,pnode_t>* pnode )
{
	_os << pnode->head << ' ';
	if( !_flatten ) _os << '[';
	_print( pnode->tail );
	if( !_flatten ) _os << ']';
}

void Printer::_print( const Node<value_t,value_t>* pnode )
{
	_os << pnode->head << ' ' << pnode->tail;
}

void Printer::_print( pnode_t pnode )
{
	switch( pnode.typecode() )
	{
		case Node<pnode_t,pnode_t>::TYPECODE:
			_print( pnode.cast<pnode_t,pnode_t>() );
			break;
		case Node<pnode_t,value_t>::TYPECODE:
			_print( pnode.cast<pnode_t,value_t>() );
			break;
		case Node<value_t,pnode_t>::TYPECODE:
			_print( pnode.cast<value_t,pnode_t>() );
			break;
		case Node<value_t,value_t>::TYPECODE:
			_print( pnode.cast<value_t,value_t>() );
			break;
		default:
			error( "<< dispatch failed" );
	}
}