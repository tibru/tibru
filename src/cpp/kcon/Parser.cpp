#include "Parser.h"


pnode_t Parser::_parse_elems( std::istream& is )
{
	char c;
	if( is >> c )
	{
		if( c == ']' )
		{
			return pnode_t::null();
		}
		else if( c == '[' )
		{
			pnode_t elems = _parse_elems( is );
			pnode_t tail = _parse_elems( is );
			return new (_alloc) Node<pnode_t,pnode_t>{ elems, tail };
		}
		else if( isdigit( c ) )
		{
			is.putback( c );
			value_t value;
			if( (is >> value) && (value < 256) )
			{
				pnode_t tail = _parse_elems( is );
				return new (_alloc) Node<value_t,pnode_t>{ value, tail };
			}
			
			error( "Malformed byte" );	
		}
		
		error( "Unexpected '%c'", c );
	}
	
	error( "Unexpected end of input" );
	return pnode_t::null();
}

pnode_t Parser::parse( std::istream& is )
{
	char c;
	if( is >> c )
	{
		assert( c == '[', "Expected '['" );
		return _parse_elems( is );
	}
	
	error( "Unexpected end of input" );
	
	return pnode_t::null();
}