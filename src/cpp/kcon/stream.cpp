#include "stream.h"
#include <stack>

kostream& kostream::operator<<( pcell_t pcell )
{
	_os << '[';
	_format( pcell );
	_os << ']';
	return *this;
}

kostream& kostream::operator<<( value_t value )
{
	_format( value );
	return *this;
}

kostream& kostream::operator<<( elem_t elem )
{
    if( elem.is_cell )
        return kostream::operator<<( elem.pcell );
    else
        return kostream::operator<<( elem.value );
}

void kostream::_format( const Cell<pcell_t,pcell_t>* pcell )
{
	_os << '[';
	_format( pcell->head );
	_os << "] ";
	if( !_flatten ) _os << '[';
	_format( pcell->tail );
	if( !_flatten ) _os << ']';
}

void kostream::_format( const Cell<pcell_t,value_t>* pcell )
{
	_os << '[';
	_format( pcell->head );
	_os << "] " << pcell->tail;
}

void kostream::_format( const Cell<value_t,pcell_t>* pcell )
{
	_os << pcell->head << ' ';
	if( !_flatten ) _os << '[';
	_format( pcell->tail );
	if( !_flatten ) _os << ']';
}

void kostream::_format( const Cell<value_t,value_t>* pcell )
{
	assert( false, "" );
}

void kostream::_format( pcell_t pcell )
{
    std::stack<elem_t> tails;
    elem_t tail = pcell;

    while( true )
    {
        if( !tail.is_cell )
        {
            _os << tail.value;
            //if( !_flatten ) _os << ']';

            if( tails.empty() )
                return;

            _os << "] ";
            tail = tails.top(); tails.pop();
        }
        else
        {
            assert( !tail.pcell.is_null(), "Null tail whilst printing" );

            switch( tail.pcell.typecode() )
            {
                case Cell<pcell_t,pcell_t>::TYPECODE:
                {
                    auto p = tail.pcell.cast<pcell_t,pcell_t>();
                    tails.push( p->tail );

                    _os << '[';
                    tail = p->head;
                    break;
                }
                case Cell<pcell_t,value_t>::TYPECODE:
                {
                    auto p = tail.pcell.cast<pcell_t,value_t>();
                    tails.push( p->tail );

                    _os << '[';
                    tail = p->head;
                    break;
                }
                case Cell<value_t,pcell_t>::TYPECODE:
                {
                    auto p = tail.pcell.cast<value_t,pcell_t>();

                    _os << p->head << ' ';
                    tail = p->tail;
                    break;
                }
                case Cell<value_t,value_t>::TYPECODE:
                {
                    auto p = tail.pcell.cast<value_t,value_t>();

                    //if( !_flatten ) _os << '[';
                    _os << p->head << ' ';
                    tail = p->tail;
                    break;
                }
                default:
                    throw Error<Runtime>( "format dispatch failed" );
            }
        }
    }
}

void kostream::_format( value_t value )
{
    _os << value;
}

value_t kistream::_parse_value()
{
    value_t value;
    if( !(_is >> value) || (value >= 256) )
        throw Error<Syntax>( "Malformed byte" );

    return value;
}

pcell_t kistream::_parse_elems()
{
	pcell_t tail = pcell_t::null();
	std::stack<pcell_t> tails;

	char c;
	while( _is >> c )
	{
		if( c == ']' )
		{
		    if( tail.is_null() )
                throw Error<Syntax>( "Unexpected empty cell" );

		    if( is_singleton( tail ) )
                throw Error<Syntax>( "Unexpected singleton" );

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
			_is.putback( c );
			tail = new (_alloc) Cell<value_t,pcell_t>{ _parse_value(), tail };
		}
		else
			throw Error<Syntax>( "Unexpected '"s + c + "'" );
	}

	throw Error<Syntax>( "Unexpected end of input" );
}

pcell_t kistream::_reverse_and_reduce( pcell_t pcell )
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

elem_t kistream::_parse()
{
	char c;
	if( !(_is >> c) )
        throw Error<Syntax>( "Unexpected end of input" );

    if( c == '[' )
    {
        return _reverse_and_reduce( _parse_elems() );
    }
    else if( isdigit( c ) )
    {
        _is.putback( c );
        return _parse_value();
    }
    else
        throw Error<Syntax>( "Unexpected '"s + c + "'" );
}

kistream& kistream::operator>>( elem_t& elem )
{
    elem = _parse();
    return *this;
}
