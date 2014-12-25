#include "stream.h"
#include "memory.h"
#include "runtime.h"
#include <stack>

using namespace elpa;

template<class System, MetaScheme class SchemeT>
auto elpa_ostream<System, SchemeT>::operator<<( pcell_t pcell ) -> elpa_ostream&
{
	_os << '[';
	_format( pcell );
	_os << ']';
	return *this;
}

template<class System, MetaScheme class SchemeT>
auto elpa_ostream<System, SchemeT>::operator<<( byte_t value ) -> elpa_ostream&
{
	_format( value );
	return *this;
}

template<class System, MetaScheme class SchemeT>
auto elpa_ostream<System, SchemeT>::operator<<( elem_t elem ) -> elpa_ostream&
{
    if( elem.is_pcell() )
        return elpa_ostream::operator<<( elem.pcell() );
    else
        return elpa_ostream::operator<<( elem.byte() );
}

//complicated but avoids recursion on c-stack
template<class System, MetaScheme class SchemeT>
auto elpa_ostream<System, SchemeT>::_format( pcell_t pcell )
{
    std::stack<Tail> tails;
    Tail tail{ pcell, 0 };

    while( true )
    {
        if( tail.elem.is_byte() || tail.elem.is_undef() )
        {
            if( tail.elem.is_undef() )
                _os << "<undef>";
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

template<class System, MetaScheme class SchemeT>
auto elpa_ostream<System, SchemeT>::_format( byte_t value )
{
    _os << short(value);
}

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
auto elpa_istream<System, SchemeT, AllocatorT>::_parse_name() -> std::string
{
	std::string name;
	char c;
	while( _is.get( c ) && isalnum( c ) )
		name += c;
	
	if( _is )
		_is.putback( c );
		
	return name;
}

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
auto elpa_istream<System, SchemeT, AllocatorT>::_parse_byte() -> byte_t
{
    value_t value;
    if( !(_is >> value) || (value >= 256) )
        throw Error<Syntax>( "Malformed byte" );

    return static_cast<byte_t>( value );
}

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
auto elpa_istream<System, SchemeT, AllocatorT>::_parse_elems( std::vector< std::string >& named ) -> elem_t
{
    auto_root<elem_t> tail( _alloc );
    elpa_stack<elem_t> tails( _alloc );

	char c;
	while( _is >> c )
	{
		if( c == ']' )
		{
		    if( tail.is_undef() )
                throw Error<Syntax>( "Unexpected empty cell" );

		    if( tail->tail().is_undef() )
                throw Error<Syntax>( "Unexpected singleton" );

			if( tails.empty() )
				return tail;

			elem_t elems = tail;

			tail = tails.top();
			tails.pop();
			tail = _alloc.new_Cell( elems, tail );
		}
		else if( c == '[' )
		{
			tails.push( tail );
			tail = elem_t();
		}
		else if( isalpha( c ) )
		{
			_is.putback( c );
			named.push_back( _parse_name() );
			tail = _alloc.new_Cell( elem_t(), tail );
		}
		else if( isdigit( c ) )
		{
			_is.putback( c );
			tail = _alloc.new_Cell( _parse_byte(), tail );
		}
		else
			throw Error<Syntax>( "Unexpected '"s + c + "'" );
	}

	throw Error<Syntax,EOS>( "Unexpected end of input" );
}

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
auto elpa_istream<System, SchemeT, AllocatorT>::_reverse_and_reduce( elem_t e, const std::vector< std::string >& named  ) -> elem_t
{
	auto pname = named.rbegin();
	
    auto_root<elem_t> p( _alloc, e );
    auto_root<elem_t> tail( _alloc );
	elpa_stack<elem_t> tails( _alloc );
	elpa_stack<elem_t> pcells( _alloc );

	std::vector<elem_t*> roots = {};
    roots.insert( roots.end(), {&p,&tail} );

    while( !pcells.empty() || p.is_def() )
    {
        if( p.is_undef() )
        {
        	System::assert( tail.is_pcell(), "Expected recursive cell tail" );

            pcell_t head = tail.pcell();

            p = pcells.top(); pcells.pop();
            tail = tails.top(); tails.pop();

			if( tail.is_undef() )
				tail = head;
			else if( tail.is_byte() )
				tail = _alloc.new_Cell( head, tail.byte() );
			else
            	tail = _alloc.new_Cell( head, tail.pcell() );
        }
        else
        {
            System::assert( !p->tail().is_byte(), "Expected tail not to be a byte in reverse and reduce" );

            if( p->head().is_pcell() )
            {
                pcells.push( p->tail() );
                tails.push( tail );

                p = p->head();
                tail = elem_t();
            }
            else if( p->head().is_undef() )
            {
            	const std::string name = *pname++;
            	try
            	{
            		const elem_t head = _names.at( name );

                	if( tail.is_undef() )
                 	   tail = head;
                	else if( tail.is_byte() )
                 	   tail = _alloc.new_Cell( head, tail.byte() );
                	else
                    	tail = _alloc.new_Cell( head, tail.pcell() );

                	p = p->tail();
            	}
            	catch( const std::out_of_range& )
    			{
    				throw Error<Syntax,Undef>( "Undefined reference to '"s + name + "'" );
    			}
            }
            else if( p->head().is_byte() )
            {
                const byte_t head = p->head().byte();

                if( tail.is_undef() )
                    tail = head;
                else if( tail.is_byte() )
                    tail = _alloc.new_Cell( head, tail.byte() );
                else
                    tail = _alloc.new_Cell( head, tail.pcell() );

                p = p->tail();
            }
            else
            	System::assert( false, "Unhandled cell type in reverse and reduce" );
        }
    }

    System::assert( tails.empty(), "Cell and tail stack mismatch" );
	return tail;
}

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
auto elpa_istream<System, SchemeT, AllocatorT>::_parse() -> elem_t
{
	char c;
	if( !(_is >> c) )
        throw Error<Syntax>( "Unexpected end of input" );
	
    if( c == '[' )
    {
    	std::vector< std::string > named;
    	auto elems = _parse_elems( named );
        return _reverse_and_reduce( elems, named );
    }
    else if( isalpha( c ) )
    {
    	_is.putback( c );
    	std::string name = _parse_name();
    	try
    	{
        	return _names.at( name );
    	}
    	catch( const std::out_of_range& )
    	{
    		throw Error<Syntax,Undef>( "Undefined reference to '"s + name + "'" );
    	}
    }
    else if( isdigit( c ) )
    {
        _is.putback( c );
        return _parse_byte();
    }
    else
        throw Error<Syntax>( "Unexpected '"s + c + "'" );
}

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
auto elpa_istream<System, SchemeT, AllocatorT>::operator>>( elem_t& elem ) -> elpa_istream&
{
    elem = _parse();
    return *this;
}

template class elpa_ostream< Debug, SimpleScheme >;
template class elpa_istream< Debug, SimpleScheme, SimpleAllocator >;
template class elpa_istream< Debug, SimpleScheme, TestAllocator >;