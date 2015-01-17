#include "shell.h"

using namespace kcon;
using namespace elpa;

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
auto KConShellManager<System, SchemeT, AllocatorT>::readers() -> const Readers&
{
    static Readers readers = {
        { '#', []( Allocator& alloc, std::istream& is ) -> elem_t {
            uint32_t n = 0;
            char c = '\0';
            while( is.get(c) && isdigit(c) )
            {
                uint32_t d = n * 10 + (c - '0');
                if( d < n )
                    throw Error<Syntax>( "Integer overflow for #" );
                n = d;
            }
            if( !isdigit(c) && c != '\0' )
                is.putback( c );

            auto_root<elem_t> n4( alloc );
            n4 = alloc.new_Cell( byte_t((n >>  0) & 0xff), n4 );
            n4 = alloc.new_Cell( byte_t((n >>  8) & 0xff), n4 );
            n4 = alloc.new_Cell( byte_t((n >> 16) & 0xff), n4 );
            n4 = alloc.new_Cell( byte_t((n >> 24) & 0xff), n4 );

            return n4;
        } },
    };

    return readers;
}

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
auto KConShellManager<System, SchemeT, AllocatorT>::macros() -> const Macros&
{
    static Macros macros = {
        { '\'', []( Allocator& alloc, elem_t tail ) -> elem_t {
        	if( !tail.is_pcell() )
        		throw Error<Syntax>( "Invalid application of '" );

        	auto_root<elem_t> t( alloc, tail );
            auto_root<elem_t> r( alloc );
            r = alloc.new_Cell( byte_t(0), r );
            r = alloc.new_Cell( t.pcell()->head(), r );
            return alloc.new_Cell( r, t.pcell()->tail() );
        } },
        { '<', []( Allocator& alloc, elem_t tail ) -> elem_t {
			auto_root<elem_t> t( alloc, tail );
            auto_root<elem_t> r( alloc );
            while( t.is_pcell() )
            {
            	r = alloc.new_Cell( t.pcell()->head(), r );
                t = t.pcell()->tail();
            }
			return r;
        } },
    };

    return macros;
}

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
auto KConShellManager<System, SchemeT, AllocatorT>::operators() -> const Operators&
{
    static const Operators ops = {
        "![[f x] v]        -> ![*[v f] *[v x]]",
        "![0 x]            -> x",
        "![1 e [x y]]      -> ?[[x y] c]",
        "![2 x r y]        -> +[x r y]",
        "*[v [[x y] .. z]] -> [@[v z] @[v [x y]]]",
        "*[v e]            -> @[v e]",
        "@[v 0 x]          -> .x",
        "@[v 1 r]          -> /[v r]",
        ".x                -> x",
        "/[v [t1 ..] h1]   -> head{h1}( tail{t1 + 256*t2 ...}(v) )",
        "/[v [t1 ..] h1 r] -> /[head{h1}( tail{t1 + 256*t2 ...}(v) ) r]",
        "?[[x y] [a b]]    -> y",
        "?[[x y] a]        -> x",
        "+[x r y]          -> append y onto x at r"
    };
    return ops;
}

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
void KConShellManager<System, SchemeT, AllocatorT>::print_help( elpa_ostream<System,SchemeT>& eos )
{
    eos << "\nReaders:\n";
    eos << "# - Convert unsigned integer into [b1 b2 b3 b3] format e.g. #1000 -> [232 3 0 0]\n";

    eos << "\nMacros:\n";
    eos << "' - Convert <expr> into [0 <expr>] e.g. 3' -> [0 3]\n";
    eos << "< - Reverse preceeding list elements e.g. [1 2 3 < 4 5] -> [3 2 1 < 4 5]\n";
}

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
auto KConShellManager<System, SchemeT, AllocatorT>::process_operator( char op, elem_t elem ) -> elem_t
{
    if( op == '.' )
    {
        return this->_interpreter.constant( elem );
    }
    else if( op == '/' )
    {
        return this->_interpreter.select( elem );
    }
    else if( op == '?' )
    {
        return this->_interpreter.ifcell( elem );
    }
    else if( op == '@' )
    {
        return this->_interpreter.reduce( elem  );
    }
    else if( op == '*' )
    {
        return this->_interpreter.evaluate( elem  );
    }
    else if( op == '!' )
    {
        if( _tracing )
        {
            bool more = true;
            while( more )
                elem = this->_interpreter.execute_trace( elem, more );

            return elem;
        }

        return this->_interpreter.execute( elem  );
    }

    throw Error<Syntax>( "Unimplemented operator '"s + op + "'" );
}

#include "../elpa/runtime.h"
template class KConShellManager<Debug, SimpleScheme, TestAllocator>;
template class KConShellManager<Debug, SimpleScheme, SimpleAllocator>;
template class KConShellManager<Safe, OptScheme, OptAllocator>;
template class KConShellManager<Fast, OptScheme, OptAllocator>;
