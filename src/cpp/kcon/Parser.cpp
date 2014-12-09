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
				throw SyntaxError( "Malformed byte" );

			tail = new (_alloc) Cell<value_t,pcell_t>{ value, tail };
		}
		else
			SyntaxError( _ << "Unexpected '" << c << "'" );
	}

	throw SyntaxError( "Unexpected end of input" );
}

pcell_t Parser::_reverse_and_reduce( pcell_t pcell )
{
    elem_t tail = pcell_t::null();
	std::stack<elem_t> tails;
	std::stack<pcell_t> pcells;

    while( !pcell.is_null() || !pcells.empty() )
    {
        if( pcell.is_null() )
        {
        	assert( tail.is_cell, "Expected recursive cell tail" );
            
            pcell_t rhead = tail.pcell;

            pcell = pcells.top(); pcells.pop();
            tail = tails.top(); tails.pop();

			if( tail.is_null() )
				tail = rhead;
			else if( !tail.is_cell )
				tail = new (_alloc) Cell<pcell_t,value_t>{ rhead, tail.value };
			else
            	tail = new (_alloc) Cell<pcell_t,pcell_t>{ rhead, tail.pcell };
        }

        switch( pcell.typecode() )
        {
            case Cell<pcell_t,pcell_t>::TYPECODE:
            {
                const Cell<pcell_t,pcell_t>* pcc = pcell.cast<pcell_t,pcell_t>();

                pcells.push( pcc->tail );
                tails.push( tail );

                pcell = pcc->head;
                tail = pcell_t::null();
                break;
            }
            case Cell<value_t,pcell_t>::TYPECODE:
            {
                const Cell<value_t,pcell_t>* pvc = pcell.cast<value_t,pcell_t>();

				const value_t value = pvc->head;
				
				if( tail.is_null() )
					tail = value;
				else if( !tail.is_cell )
					tail = new (_alloc) Cell<value_t,value_t>{ value, tail.value };
				else
					tail = new (_alloc) Cell<value_t,pcell_t>{ value, tail.pcell };
					
                pcell = pvc->tail;

                break;
            }
            default:
                assert( false, "Dispatch failed in reverse and reduce" );
        }
    }

    assert( tails.empty(), "Cell and tail stack mismatch" );

	return tail.pcell;	//check
}

pcell_t Parser::parse( std::istream& is )
{
	char c;
	if( is >> c )
	{
		assert( c == '[', "Expected '['" );
		return _reverse_and_reduce( _parse_elems( is ) );
	}

	throw SyntaxError( "Unexpected end of input" );
}