#include "tests.h"
#include "interpreter.h"
#include "shell.h"
#include "../elpa/runtime.h"

using namespace elpa;

namespace kcon {

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

    static auto parse( Allocator& allocator, const std::string& in, const Defns& defns, const Readers& readers, const Macros& macros ) -> auto_root_ref<elem_t>
    {
        std::istringstream iss( in );
        elem_t elem;
        elpa_istream( iss, allocator, defns, readers, macros ) >> elem;
        return auto_root_ref<elem_t>( allocator, elem );
    }

    static auto parse( Allocator& allocator, const std::string& in ) -> auto_root_ref<elem_t>
    {
    	Defns defns( allocator );
    	Readers readers = ShellManager::readers();
    	Macros macros = ShellManager::macros();
    	return parse( allocator, in, defns, readers, macros );
    }

    static auto print( elem_t e ) -> std::string
    {
        std::ostringstream oss;
        elpa_ostream( oss ) << e;
        return oss.str();
    }

    static void test_parse()
    {TEST
        Allocator a( 1024 );
        test( print( parse( a, "#1000" ) ) == "[232 3 0 0]", "Incorrect parse of #" );

        try
        {
            parse( a, "#40000000000" );
            fail( "Parsed integer larger that 32bits with #" );
        }
        catch( Error<Syntax> ) { pass(); }

        auto test_parse = [&]( const std::string& in, const std::string& out )
        {
            elem_t elem = parse( a, in );

            std::string found = print( elem );

            test( found == out, "Parse failed for: '" + in + "'\nExpected: '" + out + "'\nFound:    '" + found + "'" );
        };

        test_parse( "[1 2 < 3]", "[2 1 3]" );
        test_parse( "[1 2 3 <]", "[3 2 1]" );
        test_parse( "[1 2 3 < 4 5]", "[3 2 1 4 5]" );
        test_parse( "[[1 1] [2 2] [3 3] <]", "[[3 3] [2 2] 1 1]" );
        test_parse( "1 <", "1" );
        test_parse( "[[1 2 < 3] 2 [0 < 1] [10 20 <] <]", "[[20 10] [0 1] 2 2 1 3]" );

        test_parse( "[1'' 9]", "[[0 0 1] 9]" );
    }

    static void test_operators()
    {TEST
        Shell< Env > shell( 1024 );

        auto test_op = [&]( const std::string& in, const std::string& out )
        {
            std::string found = print( shell.process( in ) );

            test( found == out, "Op failed for: '" + in + "'\nExpected: '" + out + "'\nFound:    '" + found + "'" );
        };

        auto test_op_illegal = [&]( const std::string& in, const std::string& msg )
        {
            try
            {
                std::string found = print( shell.process( in ) );
                fail( "Op failed for: '" + in + "'\nExpected error: '" + msg + "'\nFound:    '" + found + "'" );
            }
            catch( const Error<IllegalOp>& e )
            {
                test( e.message() == msg, "Op failed for: '" + in +" '\nExpected error: '" + msg + "'\nFound:          '" + e.message() + "'" );
                pass();
            }
        };

        shell.process( ":def const 0" );
        shell.process( ":def select 1" );

        //Constant
        test_op( ".[0 21]", "[0 21]" );

        //Select
        test_op( "/[[1 0] 0]", "[[1 0] 0]");

        //Reduce
        test_op_illegal( "@21", "@ operates only on pairs" );
        test_op_illegal( "@[2 3]", "@ requires head element to be 0 or 1" );
        test_op( "@[const 21]", "21" );
        test_op( "@[select #1 0]", "[[1 0 0 0] 0]" );
    }

    static void run_tests()
    {
        std::cout << "TEST: " << TYPENAME( Env );

        test_parse();
        test_operators();

        std::cout << "\n\n";
    }
};

void run_tests()
{
    Tester< Env< Debug, SimpleScheme, TestAllocator, KConInterpreter > >::run_tests();
    Tester< Env< Debug, SimpleScheme, SimpleAllocator, KConInterpreter > >::run_tests();
}

}	//namespace
