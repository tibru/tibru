#ifndef KCON_TYPES_H
#define KCON_TYPES_H

#include <cstdint>
#include "common.h"

namespace kcon {

template<class T> struct Tag;

typedef uintptr_t slot_t;
const slot_t TAG_MASK = 2 * sizeof(slot_t) - 1;
const slot_t ADDR_MASK = ~TAG_MASK;
const slot_t TAIL_MASK = 1;
const slot_t HEAD_MASK = 2;
const slot_t TYPE_MASK = HEAD_MASK | TAIL_MASK;
//const slot_t MARK_ADDR_BIT = 1 << 2;
//const slot_t MARK_BYTE_BIT = 1 << 8;
//const slot_t MARK_MASK = MARK_BYTE_BIT | MARK_ADDR_BIT;

typedef uint8_t byte_t;
typedef uintptr_t value_t;
typedef const struct Cell* pcell_t;

template<class T>
T null();

template<>
inline pcell_t null()
{
    return reinterpret_cast<const Cell*>( 256 );
}

class elem_t
{
    union {
        value_t _value;
        pcell_t _pcell;
    };
public:
    elem_t( byte_t b ) : _value( b ) {}
    elem_t( pcell_t p=null<pcell_t>() ) : _pcell( p ) {}

    bool is_byte() const { return _value < 256; }
    bool is_pcell() const { return !is_byte(); }
    bool is_null() const { return is_pcell() && (_pcell == null<pcell_t>()); }

    byte_t byte() const
    {
        assert( is_byte(), "elem_t is not a byte" );
        return static_cast<byte_t>( _value );
    }

    pcell_t pcell() const
    {
        assert( is_pcell(), "elem_t is not a pcell" );
        return _pcell;
    }
};

ASSERT( sizeof(value_t) == sizeof(void*) );
ASSERT( sizeof(pcell_t) == sizeof(void*) );
ASSERT( sizeof(elem_t) == sizeof(void*) );

class Cell
{
	const elem_t _head;
	const elem_t _tail;
public:
    Cell( elem_t head, elem_t tail )
        : _head( head ), _tail( tail ) {}

    elem_t head() const { return _head; }
    elem_t tail() const { return _tail; }
};

inline bool is_singleton( pcell_t p )
{
    return (p != null<pcell_t>()) && p->tail().is_null();
}

}	//namespace

/*
Scheme 1
Combines selector bits of address and type of cell
Extends to 64 bit with upto 8 packed bytes per cell

addr = hi bits of address
sl = 2-bit byte selector (32bit)
psl = selector of pointer address (ie can point into a cell)

Cell:
|addr|0|sl| |addr|0|sl|   -> [<addr|0|sl> <addr|0|sl>]
|addr|0|sl| |cont|1|00|   -> [<addr|0|sl> ...]  ('cont' contiguous cells of type [[] ...]

|addr|1|sl| |b4b3b2b1|    -> psl=3:[b4 [b3 [b2 [b1 <addr|0|sl>]]]], psl=2:[b3 [b2 [b1 <addr|0|sl>]]], psl=1:[b2 [b1 <addr|0|sl>], psl=0:[b1 <addr|0|sl>]
|<pge|1|00| |b4b3b2b1|    -> psl=3:[b4 [b3 [b2 [b1 ...]]]],         psl=2:[b3 [b2 [b1 ...]]],         psl=1:[b2 [b1 ...]],        psl=0:[b1 ...]           (small addresses assumed to be cell count)
|0000|1|00| |b4b3b2b1|    -> psl=3:[b4 [b3 [b2 b1]]],               psl=2:[b3 [b2 b1]],               psl=1:[b2 b1],              psl=0:[b1]

NB [[] b] => [[] [b]] ie [<addr|0|sl> <addr|0|00>] + |0000|1|00| |xxxxxxb1|

pcell_t:
|addr|0|sel|

elem_t:
|addr|0|sel|
|b1| < 256

Scheme 2
More compact but taking tails requires heap allocations to unpack
All addresses are cell aligned

Cell:
|addr|000| |addr|xxx|   -> [<addr|000> <addr|000>]
|addr|001| |      b1|   -> [<addr|000> b1]
|addr|010| |      b1|   -> [b1 <addr|000>]
|0000|011| |    b2b1|   -> [b2 b1]
|addr|011| |    b2b1|   -> [b2 [b1 <addr|000>]]
|addr|100| |b4b3b2b1|   -> [[b4 [b3 [b2 b1]]] <addr|000>]
|0000|100| |b4b3b2b1|   -> [b4 [b3 [b2 b1]]]
|cont|101| |b4b3b2b1|   -> [b4 [b3 [b2 [b1 ...]]]]
|cont|110| |b4b3b2b1|   -> [[b4 [b3 [b2 b1]]] ...]
|cont|111| |      b1|   -> [b1 ...]
|cont|111| |addr|000|   -> [<addr|000> ...]

pcell_t:
|addr|000|

elem_t:
|addr|000|
|b1| < 256
*/

#endif
