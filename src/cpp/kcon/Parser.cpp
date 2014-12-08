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
			tail = new (_alloc) Cell<pcell_t,pcell_t>{ elems, tail };
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

			tail = new (_alloc) Cell<value_t,pcell_t>{ value, tail };
		}
		else
			error( "Unexpected '%c'", c );
	}

	error( "Unexpected end of input" );
	return pcell_t::null();
}

pcell_t Parser::_reverse_and_reduce( pcell_t pcell )
{
	switch( pcell.typecode() )
	{
		case Cell<pcell_t,pcell_t>::TYPECODE:
		{
            const Cell<pcell_t,pcell_t>* pcc = pcell.cast<pcell_t,pcell_t>();
			return new (_alloc) Cell<pcell_t,pcell_t>{ pcc->head, pcc->tail };
			break;
		}
		case Cell<pcell_t,value_t>::TYPECODE:
		{
            const Cell<pcell_t,value_t>* pcv = pcell.cast<pcell_t,value_t>();
			return new (_alloc) Cell<pcell_t,value_t>{ pcv->head, pcv->tail };
			break;
		}
		case Cell<value_t,pcell_t>::TYPECODE:
		{
            const Cell<value_t,pcell_t>* pvc = pcell.cast<value_t,pcell_t>();
			return new (_alloc) Cell<value_t,pcell_t>{ pvc->head, pvc->tail };
			break;
		}
		case Cell<value_t,value_t>::TYPECODE:
		{
            const Cell<value_t,value_t>* pvv = pcell.cast<value_t,value_t>();
			return new (_alloc) Cell<value_t,value_t>{ pvv->head, pvv->tail };
			break;
		}
		default:
			error( "dispatch failed" );
	}

	return pcell_t::null();
}

pcell_t Parser::parse( std::istream& is )
{
	char c;
	if( is >> c )
	{
		assert( c == '[', "Expected '['" );
		return _reverse_and_reduce( _parse_elems( is ) );
	}

	error( "Unexpected end of input" );

	return pcell_t::null();
}
