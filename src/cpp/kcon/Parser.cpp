#include "Parser.h"

#include <stack>

pnode_t Parser::_parse_elems( std::istream& is )
{
	pnode_t tail = pnode_t::null();
	std::stack<pnode_t> tails;
	
	char c;
	while( is >> c )
	{
		if( c == ']' )
		{
			if( tails.empty() )
				return tail;
				
			pnode_t elems = tail;
			tail = tails.top();
			tails.pop();
			tail = new (_alloc) Node<pnode_t,pnode_t>{ elems, tail };
		}
		else if( c == '[' )
		{
			tails.push( tail );
			tail = pnode_t::null();
		}
		else if( isdigit( c ) )
		{
			is.putback( c );
			value_t value;
			if( !(is >> value) || (value >= 256) )
				error( "Malformed byte" );
			
			tail = new (_alloc) Node<value_t,pnode_t>{ value, tail };
		}
		else
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