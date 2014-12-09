#ifndef HEADER_KCON_PARSER
#define HEADER_KCON_PARSER

#include "Allocator.h"
#include <istream>

/* Parsing avoids recursive descent simplicity to prevent c-stack overflow */
class Parser
{
	Allocator& _alloc;

    static bool _is_singleton( pcell_t p )
    {
        return (p.tailcode() == Tag<pcell_t>::CODE) && p.cast<value_t,pcell_t>()->tail.is_null();
    }

	pcell_t _parse_elems( std::istream& is );
	pcell_t _reverse_and_reduce( pcell_t p );
public:
	Parser( Allocator& alloc )
		: _alloc( alloc ) {}

	pcell_t parse( std::istream& is );
};

struct Syntax;
typedef Error<Syntax> SyntaxError;

#endif
