#include "stream.h"
#include "kstack.h"
#include <stack>

using namespace kcon;

kostream& kostream::operator<<( pcell_t pcell )
{
	_os << '[';
	_format( pcell );
	_os << ']';
	return *this;
}

kostream& kostream::operator<<( byte_t value )
{
	_format( value );
	return *this;
}

kostream& kostream::operator<<( elem_t elem )
{
    if( elem.is_cell() )
        return kostream::operator<<( elem.pcell() );
    else
        return kostream::operator<<( elem.byte_value() );
}

struct Tail
{
    elem_t elem;
    size_t len;
};

//complicated but avoids recursion on c-stack
void kostream::_format( pcell_t pcell )
{
    std::stack<Tail> tails;
    Tail tail{ pcell, 0 };

    while( true )
    {
        if( tail.elem.is_byte() || tail.elem.is_null() )
        {
            if( tail.elem.is_null() )
                _os << "<null>";
            else
                _format( tail.elem.byte_value() );

            if( !_flatten )
                for( size_t l = tail.len; l != 0; --l )
                    _os << ']';

            if( tails.empty() )
                return;

            _os << "] ";
            tail = tails.top();
            tails.pop();

            if( !_flatten && tail.elem.is_cell()) _os << '[';
        }
        else
        {
            switch( tail.elem.pcell().typecode() )
            {
                case Cell<pcell_t,pcell_t>::TYPECODE:
                {
                    auto p = tail.elem.pcell().cast<pcell_t,pcell_t>();
                    tails.push( Tail{ p->tail, tail.len + 1 } );

                    _os << '[';
                    tail = Tail{ p->head, 0 };
                    break;
                }
                case Cell<pcell_t,value_t>::TYPECODE:
                {
                    auto p = tail.elem.pcell().cast<pcell_t,value_t>();
                    tails.push( Tail{ p->tail, tail.len } );

                    _os << '[';
                    tail = Tail{ p->head, 0 };
                    break;
                }
                case Cell<value_t,pcell_t>::TYPECODE:
                {
                    auto p = tail.elem.pcell().cast<value_t,pcell_t>();

                    _os << p->head << ' ';
                    tail = Tail{ p->tail, tail.len + 1 };
                    if( !_flatten ) _os << '[';
                    break;
                }
                case Cell<value_t,value_t>::TYPECODE:
                {
                    auto p = tail.elem.pcell().cast<value_t,value_t>();

                    _os << p->head << ' ';
                    tail = Tail{ p->tail, tail.len };
                    break;
                }
                default:
                    throw Error<Runtime>( "format dispatch failed" );
            }
        }
    }
}

void kostream::_format( byte_t value )
{
    _os << short(value);
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
	kstack<pcell_t> tails( _alloc );

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
			tail = new ( _alloc, {&tails.items(), &elems, &tail} ) Cell<pcell_t,pcell_t>{ elems, tail };
		}
		else if( c == '[' )
		{
			tails.push( tail, {&tails.items(), &tail} );
			tail = pcell_t::null();
		}
		else if( isdigit( c ) )
		{
			_is.putback( c );
			tail = new ( _alloc, {&tails.items(), &tail} ) Cell<value_t,pcell_t>{ _parse_value(), tail };
		}
		else
			throw Error<Syntax>( "Unexpected '"s + c + "'" );
	}

	throw Error<Syntax,EOS>( "Unexpected end of input" );
}

pcell_t kistream::_reverse_and_reduce( pcell_t pcell )
{
    elem_t tail = pcell_t::null();
	std::stack<elem_t> tails;
	std::stack<pcell_t> pcells;

    while( !(pcell.is_null() && pcells.empty()) )
    {
        if( pcell.is_null() )
        {
        	assert( tail.is_cell(), "Expected recursive cell tail" );

            pcell_t head = tail.pcell();

            pcell = pcells.top(); pcells.pop();
            tail = tails.top(); tails.pop();

			if( tail.is_null() )
				tail = head;
			else if( tail.is_byte() )
				tail = new (_alloc) Cell<pcell_t,value_t>{ head, tail.byte_value() };
			else
            	tail = new (_alloc) Cell<pcell_t,pcell_t>{ head, tail.pcell() };
        }
        else
        {
            switch( pcell.typecode() )
            {
                case Cell<pcell_t,pcell_t>::TYPECODE:
                {
                    auto p = pcell.cast<pcell_t,pcell_t>();

                    pcells.push( p->tail );
                    tails.push( tail );

                    pcell = p->head;
                    tail = pcell_t::null();
                    break;
                }
                case Cell<value_t,pcell_t>::TYPECODE:
                {
                    auto* p = pcell.cast<value_t,pcell_t>();

                    const value_t value = p->head;

                    if( tail.is_null() )
                        tail = value;
                    else if( tail.is_byte() )
                        tail = new (_alloc) Cell<value_t,value_t>{ value, tail.byte_value() };
                    else
                        tail = new (_alloc) Cell<value_t,pcell_t>{ value, tail.pcell() };

                    pcell = p->tail;
                    break;
                }
                default:
                    assert( false, "Dispatch failed in reverse and reduce" );
            }
        }
    }

    assert( tails.empty(), "Cell and tail stack mismatch" );

	return tail.pcell();
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
