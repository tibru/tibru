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
        { '\'', []( Allocator& alloc, elem_t elem ) -> elem_t {
            return alloc.new_Cell( byte_t(0), elem );
        } },
        { '<', []( Allocator& alloc, elem_t elem ) -> elem_t {
            elpa_ostream<System, SchemeT> os( std::cout );
            os << "Tail: " << elem << std::endl;

            auto_root<elem_t> r( alloc );
            while( elem.is_pcell() )
            {
                r = alloc.new_Cell( elem.pcell()->head(), r );
                elem = elem.pcell()->tail();
            }
            return r;
        } },
    };

    return macros;
}

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
auto KConShellManager<System, SchemeT, AllocatorT>::operators() -> const std::vector<char>&
{
    static std::vector<char> ops = {'!'};
    return ops;
}

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
auto KConShellManager<System, SchemeT, AllocatorT>::process_operator( char op, elem_t elem ) -> elem_t
{
    if( op == '!' )
    {
        return elem;
    }

    return elem_t();
}

#include "../elpa/runtime.h"
template class KConShellManager<Debug, SimpleScheme, TestAllocator>;
template class KConShellManager<Debug, SimpleScheme, SimpleAllocator>;
template class KConShellManager<Safe, OptScheme, OptAllocator>;
template class KConShellManager<Fast, OptScheme, OptAllocator>;
