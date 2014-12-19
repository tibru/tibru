#include "stream.h"
#include "memory.h"
#include "runtime.h"
#include <stack>

using namespace kcon;

template<class T>
using kstack = Env<SimpleScheme, SimpleAllocator>::kstack<T>;

template<class Scheme>
auto kostream<Scheme>::operator<<( pcell_t pcell ) -> kostream<Scheme>&
{
	_os << '[';
	_format( pcell );
	_os << ']';
	return *this;
}

template<class Scheme>
auto kostream<Scheme>::operator<<( byte_t value ) -> kostream<Scheme>&
{
	_format( value );
	return *this;
}

template<class Scheme>
auto kostream<Scheme>::operator<<( elem_t elem ) -> kostream<Scheme>&
{
    if( elem.is_pcell() )
        return kostream<Scheme>::operator<<( elem.pcell() );
    else
        return kostream<Scheme>::operator<<( elem.byte() );
}

//complicated but avoids recursion on c-stack
template<class Scheme>
auto kostream<Scheme>::_format( pcell_t pcell )
{
    std::stack<Tail> tails;
    Tail tail{ pcell, 0 };

    while( true )
    {
        if( tail.elem.is_byte() || (tail.elem == null<elem_t>()) )
        {
            if( tail.elem == null<elem_t>() )
                _os << "<null>";
            else
                _format( tail.elem.byte() );

            if( !_flatten )
                for( size_t l = tail.len; l != 0; --l )
                    _os << ']';

            if( tails.empty() )
                return;

            _os << "] ";
            tail = tails.top();
            tails.pop();

            if( !_flatten && tail.elem.is_pcell()) _os << '[';
        }
        else
        {
            auto p = tail.elem.pcell();

            if( p->head().is_pcell() && p->tail().is_pcell() )
            {
                tails.push( Tail{ p->tail(), tail.len + 1 } );

                _os << '[';
                tail = Tail{ p->head(), 0 };
            }
            else if( p->head().is_pcell() && p->tail().is_byte() )
            {
                tails.push( Tail{ p->tail(), tail.len } );

                _os << '[';
                tail = Tail{ p->head(), 0 };
            }
            else if( p->head().is_byte() && p->tail().is_pcell() )
            {
                _os << (short) p->head().byte() << ' ';
                tail = Tail{ p->tail(), tail.len + 1 };
                if( !_flatten ) _os << '[';
            }
            else
            {
                _os << (short) p->head().byte() << ' ';
                tail = Tail{ p->tail(), tail.len };
            }
        }
    }
}

template<class Scheme>
auto kostream<Scheme>::_format( byte_t value )
{
    _os << short(value);
}

template<class Scheme, class Allocator>
auto kistream<Scheme,Allocator>::_parse_byte() -> byte_t
{
    value_t value;
    if( !(_is >> value) || (value >= 256) )
        throw Error<Syntax>( "Malformed byte" );

    return static_cast<byte_t>( value );
}

template<class Scheme, class Allocator>
auto kistream<Scheme,Allocator>::_parse_elems() -> pcell_t
{
	elem_t tail = null<elem_t>();
	kstack<pcell_t> tails( _alloc );

	std::vector<elem_t*> roots = _roots;
    roots.insert( roots.end(), {&tail,&tails.items()} );

	char c;
	while( _is >> c )
	{
		if( c == ']' )
		{
		    if( tail == null<elem_t>() )
                throw Error<Syntax>( "Unexpected empty cell" );

		    if( is_singleton( tail ) )
                throw Error<Syntax>( "Unexpected singleton" );

			if( tails.empty() )
				return tail.pcell();

			pcell_t elems = tail.pcell();
			tail = tails.top();
			tails.pop();
			tail = _alloc.new_Cell( elems, tail, roots );
		}
		else if( c == '[' )
		{
			tails.push( tail, roots );
			tail = null<elem_t>();
		}
		else if( isdigit( c ) )
		{
			_is.putback( c );
			tail = _alloc.new_Cell( _parse_byte(), tail, roots );
		}
		else
			throw Error<Syntax>( "Unexpected '"s + c + "'" );
	}

	throw Error<Syntax,EOS>( "Unexpected end of input" );
}

template<class Scheme, class Allocator>
auto kistream<Scheme,Allocator>::_reverse_and_reduce( pcell_t pcell ) -> pcell_t
{
    elem_t p = pcell;
    elem_t tail = null<elem_t>();
	kstack<elem_t> tails( _alloc );
	kstack<pcell_t> pcells( _alloc );

	std::vector<elem_t*> roots = _roots;
    roots.insert( roots.end(), {&p,&tail,&tails.items(),&pcells.items()} );

    while( !((p == null<elem_t>()) && pcells.empty()) )
    {
        if( p == null<elem_t>() )
        {
        	assert( tail.is_pcell(), "Expected recursive cell tail" );

            pcell_t head = tail.pcell();

            p = pcells.top(); pcells.pop();
            tail = tails.top(); tails.pop();

			if( tail == null<elem_t>() )
				tail = head;
			else if( tail.is_byte() )
				tail = _alloc.new_Cell( head, tail.byte(), roots );
			else
            	tail = _alloc.new_Cell( head, tail.pcell(), roots );
        }
        else
        {
            assert( p->tail().is_pcell(), "Expected tail to be cell in reverse and reduce" );

            if( p->head().is_pcell() )
            {
                pcells.push( p->tail().pcell(), roots );
                tails.push( tail, roots );

                p = p->head().pcell();
                tail = null<elem_t>();
            }
            else
            {
                assert( p->head().is_byte(), "" );
                const byte_t head = p->head().byte();

                if( tail== null<elem_t>() )
                    tail = head;
                else if( tail.is_byte() )
                    tail = _alloc.new_Cell( head, tail.byte(), roots );
                else
                    tail = _alloc.new_Cell( head, tail.pcell(), roots );

                p = p->tail().pcell();
            }
        }
    }

    assert( tails.empty(), "Cell and tail stack mismatch" );
	return tail.pcell();
}

template<class Scheme, class Allocator>
auto kistream<Scheme, Allocator>::_parse() -> elem_t
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
        return _parse_byte();
    }
    else
        throw Error<Syntax>( "Unexpected '"s + c + "'" );
}

template<class Scheme, class Allocator>
auto kistream<Scheme, Allocator>::operator>>( elem_t& elem ) -> kistream<Scheme, Allocator>&
{
    elem = _parse();
    return *this;
}

template class kostream<SimpleScheme>;
template class kistream<SimpleScheme,SimpleAllocator<SimpleScheme>>;
template class kistream<SimpleScheme,TestAllocator<SimpleScheme>>;
