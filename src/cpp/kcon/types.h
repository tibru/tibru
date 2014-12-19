#ifndef KCON_TYPES_H
#define KCON_TYPES_H

#include <cstdint>
#include "common.h"

namespace kcon {

const uintptr_t TAG_MASK = 2 * sizeof(void*) - 1;
const uintptr_t ADDR_MASK = ~TAG_MASK;

template<class System>
struct SimpleScheme
{
    struct Cell;

    typedef uint8_t byte_t;
    typedef uintptr_t value_t;
    typedef const Cell* pcell_t;

    class elem_t
    {
        static const value_t UNDEF = 256;

        union {
            value_t _value;
            pcell_t _pcell;
        };
    public:
        elem_t() : _value( UNDEF ) {}
        elem_t( byte_t b ) : _value( b ) {}
        elem_t( pcell_t p ) : _pcell( p ) {}

        bool is_undef() const { return _value == UNDEF; }
        bool is_def() const { return _value != UNDEF; }
        bool is_byte() const { return _value < UNDEF; }
        bool is_pcell() const { return _value > UNDEF; }

        byte_t byte() const
        {
            System::assert( is_byte(), "elem_t is not a byte" );
            return static_cast<byte_t>( _value );
        }

        pcell_t operator->() const
        {
            return pcell();
        }

        pcell_t pcell() const
        {
            System::assert( is_pcell(), "elem_t is not a pcell" );
            return _pcell;
        }

        bool operator==( elem_t e ) const { return _value == e._value; }
        bool operator!=( elem_t e ) const { return _value != e._value; }
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
};

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
