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

class pcell_t
{
    const struct Cell* _pcell;

    pcell_t() : _pcell( reinterpret_cast<const Cell*>( 256 ) ) {}
public:
    pcell_t( const Cell* p ) : _pcell( p ) {}

    const Cell* addr() const { return _pcell; }
    const Cell* operator->() const { return _pcell; }
    bool is_null() const { return _pcell == pcell_t()._pcell; }

    short typecode() const;
    bool operator<( pcell_t q ) const { return _pcell < q._pcell; }
    bool operator==( pcell_t q ) const { return _pcell == q._pcell; }

    static pcell_t null() { return pcell_t(); }
};

struct elem_t
{
    union {
        value_t value;
        pcell_t pcell;
    };

    elem_t( byte_t b ) : value( b ) {}
    elem_t( pcell_t p=pcell_t::null() ) : pcell( p ) {}
    elem_t( const Cell* p ) : pcell( p ) {}

    bool is_byte() const { return value < 256; }
    bool is_pcell() const { return !is_byte(); }
    bool is_null() const { return is_pcell() && pcell.is_null(); }
};

ASSERT( sizeof(value_t) == sizeof(void*) );
ASSERT( sizeof(pcell_t) == sizeof(void*) );
ASSERT( sizeof(elem_t) == sizeof(void*) );

struct Cell
{
	const elem_t head;
	const elem_t tail;

	short typecode() const
	{
	    return (head.is_byte() ? 2 : 0) | (tail.is_byte() ? 1 : 0);
	}
};

inline short pcell_t::typecode() const
{
    return _pcell->typecode();
}

template<class H, class T>
struct CellType
{
	static const short TYPECODE = (Tag<H>::CODE << 1) | Tag<T>::CODE;
};

template<> struct Tag<pcell_t> { enum { CODE = 0 }; };
template<> struct Tag<value_t> { enum { CODE = 1 }; };

inline bool is_singleton( pcell_t p )
{
    return !p.is_null() && p->tail.is_null();
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
