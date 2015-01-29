#ifndef ELPA_STREAM_TPP
#define ELPA_STREAM_TPP

#include "stream.h"
#include "memory.h"
#include <stack>

namespace elpa {

template<class System, MetaScheme class SchemeT>
void elpa_ostream<System, SchemeT>::_print_elem( elem_t elem, const std::map<byte_t, std::string>& byte_names, const std::map<pcell_t, std::string>& cell_names )
{
    if( elem.is_pcell() )
        _print( elem.pcell(), byte_names, cell_names );
    else if( elem.is_byte() )
        _print( elem.byte(), byte_names );
    else
        _os << "<undef>";
}

template<class System, MetaScheme class SchemeT>
void elpa_ostream<System, SchemeT>::_print( pcell_t pcell, const std::map<byte_t, std::string>& byte_names, const std::map<pcell_t, std::string>& cell_names )
{
    _os << '[';
    if( pcell->head().is_pcell() && cell_names.find( pcell->head().pcell() ) == cell_names.end() )
    {
        _os << '[';
        _format( pcell->head().pcell(), byte_names, cell_names );
        _os << ']';
    }
    else
    {
        _format_elem( pcell->head(), byte_names, cell_names );
    }

    _os << ' ';

    if( pcell->tail().is_pcell() && cell_names.find( pcell->tail().pcell() ) == cell_names.end() && !_flatten )
    {
        _os << '[';
        _format( pcell->tail().pcell(), byte_names, cell_names );
        _os << ']';
    }
    else
    {
        _format_elem( pcell->tail(), byte_names, cell_names );
    }
    _os << ']';
}

template<class System, MetaScheme class SchemeT>
void elpa_ostream<System, SchemeT>::_print( byte_t value, const std::map<byte_t, std::string>& byte_names )
{
	_os << (short) Scheme::byte_value( value );
}

template<class System, MetaScheme class SchemeT>
void elpa_ostream<System, SchemeT>::_format_elem( elem_t elem, const std::map<byte_t, std::string>& byte_names, const std::map<pcell_t, std::string>& cell_names )
{
    if( elem.is_pcell() )
        _format( elem.pcell(), byte_names, cell_names );
    else if( elem.is_byte() )
        _format( elem.byte(), byte_names );
    else
        _os << "<undef>";
}

//complicated but avoids recursion on c-stack
template<class System, MetaScheme class SchemeT>
void elpa_ostream<System, SchemeT>::_format( pcell_t pcell, const std::map<byte_t, std::string>& byte_names, const std::map<pcell_t, std::string>& cell_names )
{
    std::stack<Tail> tails;
    Tail tail{ pcell, 0 };

    while( true )
    {
        if( tail.elem.is_byte() || tail.elem.is_undef() || cell_names.find( tail.elem.pcell() ) != cell_names.end() )
        {
            if( tail.elem.is_undef() )
                _os << "<undef>";
            else if( tail.elem.is_byte() )
                _format( tail.elem.byte(), byte_names );
            else
                _os << cell_names.at( tail.elem.pcell() );

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

            if( p->head().is_pcell() && cell_names.find( p->head().pcell() ) != cell_names.end() )
            {
                _os << cell_names.at( p->head().pcell() ) << ' ';
                if( p->tail().is_pcell() )
                {
                    tail = Tail{ p->tail(), tail.len + 1 };
                    if( !_flatten ) _os << '[';
                }
                else if( p->tail().is_byte() || p->tail().is_undef() )
                {
                    _os << ' ';
                    tail = Tail{ p->tail(), tail.len };
                }
                else
                    System::assert( false, "Found cell with tail neither cell, byte or undef" );
            }
            else if( p->head().is_pcell() && p->tail().is_pcell() )
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
                _format( p->head().byte(), byte_names );
                _os << ' ';
                tail = Tail{ p->tail(), tail.len + 1 };
                if( !_flatten ) _os << '[';
            }
            else if( p->head().is_byte() && p->tail().is_byte() )
            {
                _format( p->head().byte(), byte_names );
                _os << ' ';
                tail = Tail{ p->tail(), tail.len };
            }
            else if( p->head().is_pcell() && p->tail().is_undef() )
            {
                tails.push( Tail{ p->tail(), tail.len } );

                _os << '[';
                tail = Tail{ p->head(), 0 };
            }
            else if( p->head().is_byte() && p->tail().is_undef() )
            {
                _format( p->head().byte(), byte_names );
                _os << ' ';
                tail = Tail{ p->tail(), tail.len };
            }
            else
                System::assert( false, "Unhandled format type" );
        }
    }
}

template<class System, MetaScheme class SchemeT>
void elpa_ostream<System, SchemeT>::_format( byte_t value, const std::map<byte_t, std::string>& byte_names )
{
    auto i = byte_names.find( value );

    if( i == byte_names.end() )
        _os << (short) Scheme::byte_value( value );
    else
        _os << i->second;
}

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
auto elpa_istream<System, SchemeT, AllocatorT>::_parse_name() -> std::string
{
	std::string name;
	char c;
	while( _is.get( c ) && (isalnum( c ) || c == '_') )
		name += c;

	if( _is )
		_is.putback( c );

	return name;
}

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
auto elpa_istream<System, SchemeT, AllocatorT>::_parse_reader( char c ) -> elem_t
{
    return _readers.at(c)( _alloc, _is );
}

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
auto elpa_istream<System, SchemeT, AllocatorT>::_parse_macro( char c, elem_t tail, std::vector<std::string>& names ) -> elem_t
{
    return _macros.at(c)( _alloc, tail, names );
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
auto elpa_istream<System, SchemeT, AllocatorT>::_parse_elems( std::vector< std::string >& names, size_t depth ) -> elem_t
{
    auto_root<elem_t> tail( _alloc );
    elpa_stack<elem_t> tails( _alloc );

	char c;
	while( _is >> c )
	{
		if( c == ']' )
		{
		    if( depth == 0 )
                throw Error<Syntax>( "Unexpected ']'" );

		    if( tail.is_undef() )
                throw Error<Syntax>( "Unexpected empty cell" );

		    if( tail->tail().is_undef() )
                throw Error<Syntax>( "Unexpected singleton" );

			if( !tails.empty() )
			{
                elem_t elems = tail;

                tail = tails.top();
                tails.pop();
                tail = _alloc.new_Cell( elems, tail );
			}
			else
			{
			    System::assert( depth == 1, "No tails but depth about to end" );
			}

		    --depth;
		}
		else if( c == '[' )
		{
		    ++depth;

			tails.push( tail );
			tail = elem_t();
		}
		else if( isalpha( c ) || c == '_' )
		{
			_is.putback( c );
			names.push_back( _parse_name() );
			tail = _alloc.new_Cell( elem_t(), tail );
		}
		else if( isdigit( c ) )
		{
			_is.putback( c );
			tail = _alloc.new_Cell( _parse_byte(), tail );
		}
		else if( _readers.find( c ) != _readers.end() )
		{
		    tail =  _alloc.new_Cell( _parse_reader( c ), tail );
		}
		else if( _macros.find( c ) != _macros.end() )
		{
		    if( tail.is_undef() )
                throw Error<Syntax>( "Unexpected macro '"s + c + "'" );

		    tail = _parse_macro( c, tail, names );
		}
		else
			throw Error<Syntax>( "Unexpected '"s + c + "'" );

        if( depth == 0 )
        {
            //trailing macro
            while( true )
            {
                char c = '\0';
                while( _is.get( c ) && isspace( c ) && (c != '\n') )
                    ;

                if( _macros.find( c ) == _macros.end() )
                {
                    if( _is )
                        _is.putback( c );

                    return tail;
                }

                tail = _parse_macro( c, tail, names );
            }
        }
	}

    if( tail.is_undef() )
        throw Error<Syntax,EOS>( "Unexpected end of input" );

    if( depth != 0 )
        throw Error<Syntax,EOS>( "Unexpected end of input (unclosed pair)" );

    return tail;
}

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
auto elpa_istream<System, SchemeT, AllocatorT>::_reverse_and_reduce( elem_t e, const std::vector< std::string >& names  ) -> elem_t
{
    if( e.is_byte() )
        return e;

	auto pname = names.rbegin();

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

            	if( _defns.find( name ) == _defns.end() )
                    throw Error<Syntax,Undef>( "Undefined reference to '"s + name + "'" );

            	const elem_t head = _defns.at( name );

                if( tail.is_undef() )
                	tail = head;
                else if( tail.is_byte() )
                	tail = _alloc.new_Cell( head, tail.byte() );
                else
                    tail = _alloc.new_Cell( head, tail.pcell() );

            	p = p->tail();
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
    std::vector< std::string > names;
    auto elems = _parse_elems( names, 0 );

    return _reverse_and_reduce( elems, names );
}

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
auto elpa_istream<System, SchemeT, AllocatorT>::operator>>( elem_t& elem ) -> elpa_istream&
{
    elem = _parse();
    return *this;
}

}   //namespace

#endif
