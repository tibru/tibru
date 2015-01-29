#include "tests.h"
#include "interpreter.h"
#include "shell.h"
#include "../elpa/runtime.h"

using namespace elpa;

namespace ohno {

template<class Env>
struct Tester
{
    typedef typename Env::Allocator Allocator;
    typedef typename Env::elpa_ostream elpa_ostream;
    typedef typename Env::elpa_istream elpa_istream;
    typedef typename elpa_istream::Defns Defns;
    typedef typename elpa_istream::Readers Readers;
    typedef typename elpa_istream::Macros Macros;
    typedef typename Env::Interpreter Interpreter;
    typedef typename Shell<Env>::ShellManager ShellManager;

    typedef typename Env::Scheme::elem_t elem_t;

    template<class T>
    using auto_root_ref = typename Allocator::template auto_root_ref<T>;

    static auto parse( Allocator& allocator, const std::string& in ) -> auto_root_ref<elem_t>
    {
    	Defns defns( allocator );
    	Readers readers = ShellManager::readers();
    	Macros macros = ShellManager::macros();

        std::istringstream iss( in );
        elem_t elem;
        elpa_istream( iss, allocator, defns, readers, macros ) >> elem;
        return auto_root_ref<elem_t>( allocator, elem );
    }

    static auto print( elem_t e ) -> std::string
    {
        std::ostringstream oss;
        elpa_ostream( oss ) << e;
        return oss.str();
    }

    static void test_ohno()
    {TEST
        auto test_parse = [&]( const std::string& in, const std::string& out )
        {
            Allocator a( 256 );
            elem_t elem = parse( a, in );

            std::string found = print( elem );

            test( found == out, "Parse failed for: '" + in + "'\nExpected: '" + out + "'\nFound:    '" + found + "'" );
        };

        test_parse( "[1 2 %]", "[2 1]" );
        test_parse( "[1 2 3 %]", "[1 3 2]" );
        test_parse( "[1 2 3 4 %]", "[1 2 4 3]" );
        test_parse( "[1 2 [3 4] [5 6] %]", "[1 2 [5 6] 3 4]" );
    }

    static void run_tests()
    {
        std::cout << "TEST: " << TYPENAME( Env );

        test_ohno();

        std::cout << "\n\n";
    }
};

void run_tests()
{
    Tester< Env< Debug, SimpleScheme, TestAllocator, OhNoInterpreter > >::run_tests();
    Tester< Env< Debug, SimpleScheme, SimpleAllocator, OhNoInterpreter > >::run_tests();
}

}	//namespace
