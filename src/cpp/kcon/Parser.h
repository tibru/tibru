#ifndef HEADER_KCON_PARSER
#define HEADER_KCON_PARSER

#include "Allocator.h"
#include <istream>

class Parser
{
	Allocator& _alloc;

	pcell_t _parse_elems( std::istream& is );
	pcell_t _reverse_and_reduce( pcell_t p );
public:
	Parser( Allocator& alloc )
		: _alloc( alloc ) {}

	pcell_t parse( std::istream& is );
};

#endif
