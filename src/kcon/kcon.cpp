#include <stdio.h>
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
	H head;
	T tail;
	
	static const short TYPECODE = (Tag<H>::CODE << 1) | Tag<T>::CODE;
	
	ASSERT( sizeof(H) == sizeof(slot_t) );
	ASSERT( sizeof(T) == sizeof(slot_t) );
};

class pnode_t
{
	uintptr_t _addr_and_type;
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

struct KConInterpreter
{
	template<class H, class T>
	const Node<H,T>* alloc( H head, T tail )
	{
		return new Node<H,T>{ head, tail };
	}
};

int main( int argc, const char* argv[] )
{
	KConInterpreter kcon;
	pnode_t p = kcon.alloc<value_t,value_t>( 0, 0 );
	p.cast<value_t,value_t>();
	printf( "kcon %lx", ADDR_MASK );
}