#include "Parser.h"

#include <stack>

pcell_t Parser::_parse_elems( std::istream& is )
{
	pcell_t tail = pcell_t::null();
	std::stack<pcell_t> tails;

	char c;
	while( is >> c )
	{
		if( c == ']' )
		{
			if( tails.empty() )
				return tail;

			pcell_t elems = tail;
			tail = tails.top();
			tails.pop();
			tail = new (_alloc) Node<pcell_t,pcell_t>{ elems, tail };
		}
		else if( c == '[' )
		{
			tails.push( tail );
			tail = pcell_t::null();
		}
		else if( isdigit( c ) )
		{
			is.putback( c );
			value_t value;
			if( !(is >> value) || (value >= 256) )
				error( "Malformed byte" );

			tail = new (_alloc) Node<value_t,pcell_t>{ value, tail };
		}
		else
			error( "Unexpected '%c'", c );
	}

	error( "Unexpected end of input" );
	return pcell_t::null();
}

pcell_t Parser::parse( std::istream& is )
{
	char c;
	if( is >> c )
	{
		assert( c == '[', "Expected '['" );
		return _parse_elems( is );
	}

	error( "Unexpected end of input" );

	return pcell_t::null();
}
