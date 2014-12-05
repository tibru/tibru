#ifndef KCON_TYPES_H
#define KCON_TYPES_H

#include <cstdint>
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
	uintptr_t _addr_and_type;
	
	pnode_t()
		: _addr_and_type( 0 ) {}
public:
	template<class H, class T>
	pnode_t( const Node<H,T>* pnode )
		: _addr_and_type( reinterpret_cast<uintptr_t>( pnode ) | (Tag<H>::CODE << 1) | Tag<T>::CODE ) {}
		
	static pnode_t null() { return pnode_t(); }
	
	bool is_null() const { return _addr_and_type == 0; }

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

#endif