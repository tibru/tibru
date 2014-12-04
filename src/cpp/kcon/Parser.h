#ifndef HEADER_KCON_PARSER
#define HEADER_KCON_PARSER

#include "Allocator.h"
#include <istream>

class Parser
{
	Allocator& _alloc;
	
	pnode_t _parse_elems( std::istream& is );
public:
	Parser( Allocator& alloc )
		: _alloc( alloc ) {}
		
	pnode_t parse( std::istream& is );
};

#endif