#include <iostream>
#include "common.h"

template<class T> struct Tag;

typedef uintptr_t slot_t;
const slot_t TAG_MASK = 2 * sizeof(slot_t) - 1;
const slot_t ADDR_MASK = ~TAG_MASK;
const slot_t TYPE_MASK = 3;
const slot_t MARK_ADDR_BIT = 1 << 2;
const slot_t MARK_BYTE_BIT = 1 << 8;
const slot_t MARK_MASK = MARK_BYTE_BIT | MARK_ADDR_BIT;

ASSERT( sizeof(slot_t) == sizeof(void*) );

template<class H, class T>
struct Node
{
	const H head;
	const T tail;
	
	static const short TYPECODE = (Tag<H>::CODE << 1) | Tag<T>::CODE;
	
	ASSERT( sizeof(H) == sizeof(slot_t) );
	ASSERT( sizeof(T) == sizeof(slot_t) );
};

class pnode_t
{
	const uintptr_t _addr_and_type;
public:
	template<class H, class T>
	pnode_t( const Node<H,T>* pnode )
		: _addr_and_type( reinterpret_cast<uintptr_t>( pnode ) | (Tag<H>::CODE << 1) | Tag<T>::CODE ) {}
		
	short typecode() const
	{
		return _addr_and_type & TYPE_MASK;
	}
	
	template<class H, class T>
	const Node<H,T>* cast() const
	{
		assert( typecode() == Node<H,T>::TYPECODE, "Invalid cast" );
		return reinterpret_cast<const Node<H,T>*>( _addr_and_type & ADDR_MASK ); 
	}
};

typedef uintptr_t value_t;

ASSERT( sizeof(pnode_t) == sizeof(slot_t) );
ASSERT( sizeof(value_t) == sizeof(slot_t) );

template<> struct Tag<pnode_t> { enum { CODE = 0 }; };
template<> struct Tag<value_t> { enum { CODE = 1 }; };

struct Allocator
{
	template<class H, class T>
	const Node<H,T>* alloc( H head, T tail )
	{
		return new Node<H,T>{ head, tail };
	}
};

class Printer
{
	std::ostream& _os;
	bool _flatten;
	
	void _print( const Node<pnode_t,pnode_t>* pnode );
	void _print( const Node<pnode_t,value_t>* pnode );
	void _print( const Node<value_t,pnode_t>* pnode );
	void _print( const Node<value_t,value_t>* pnode );
	void _print( pnode_t pnode );
public:
	Printer( std::ostream& os, bool flatten=true )
		: _os( os ), _flatten( flatten ) {}

	std::ostream& operator<<( pnode_t pnode );
};

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

int main( int argc, const char* argv[] )
{
	Allocator a;
	pnode_t p = a.alloc<value_t,pnode_t>( 0, a.alloc<value_t,value_t>( 1, 2 ) );
	Printer( std::cout ) << p << '\n';
	Printer( std::cout, false ) << p << '\n';
}