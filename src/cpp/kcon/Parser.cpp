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
    pcell_t tail = pcell_t::null();
	std::stack<pcell_t> tails;
	std::stack<pcell_t> pcells;

    while( !pcell.is_null() || !pcells.empty() )
    {
        if( pcell.is_null() )
        {
            pcell_t rhead = tail;

            pcell = pcells.top(); pcells.pop();
            tail = tails.top(); tails.pop();

            tail = new (_alloc) Cell<pcell_t,pcell_t>{ rhead, tail };
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
                /*pcell_t t = pvc->tail;
                if( (t.typecode() == Cell<value_t,pcell_t>::TYPECODE) && t.cast<value_t,pcell_t>()->tail.is_null() )
                {
                    tail = new (_alloc) Cell<value_t,value_t>{ pvc->head, t.cast<value_t,pcell_t>()->head };
                    pcell = pcell_t::null();
                }
                else if( (t.typecode() == Cell<pcell_t,pcell_t>::TYPECODE) && t.cast<pcell_t,pcell_t>()->tail.is_null() )
                {
                    tail = new (_alloc) Cell<value_t,pcell_t>{ pvc->head, t.cast<pcell_t,pcell_t>()->head };
                    pcell = pcell_t::null();
                }
                else*/
                {
                    tail = new (_alloc) Cell<value_t,pcell_t>{ pvc->head, tail };
                    pcell = pvc->tail;
                }

                break;
            }
            default:
                error( "Dispatch failed in reverse and reduce" );
        }
    }

    assert( tails.empty(), "Cell and tail stack mismatch" );

	return tail;
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
