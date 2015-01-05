#include "tests.h"
#include "interpreter.h"
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
    typedef typename Env::Interpreter Interpreter;

    typedef typename Env::Scheme::elem_t elem_t;

    template<class T>
    using auto_root_ref = typename Allocator::template auto_root_ref<T>;

    static auto parse( Allocator& allocator, const std::string& in, const Defns& defns ) -> auto_root_ref<elem_t>
    {
        std::istringstream iss( in );
        elem_t elem;
        elpa_istream( iss, allocator, defns ) >> elem;
        return auto_root_ref<elem_t>( allocator, elem );
    }

    static auto parse( Allocator& allocator, const std::string& in ) -> auto_root_ref<elem_t>
    {
    	Defns defns( allocator );
    	return parse( allocator, in, defns );
    }

    static auto print( elem_t e ) -> std::string
    {
        std::ostringstream oss;
        elpa_ostream( oss ) << e;
        return oss.str();
    }

    static void test_interpreter()
    {TEST
        Interpreter interpreter( 1024 );

        auto test_op = [&]( char op, const std::string& in, const std::string& out )
        {
            elem_t elem = parse( interpreter.allocator(), in );

            std::string found = print( interpreter.process_operator( op, elem ) );

            test( found == out, "Op "s + op + " failed for: '" + in + "'\nExpected: '" + out + "'\nFound:    '" + found + "'" );
        };

        auto test_op_illegal = [&]( char op, const std::string& in, const std::string& msg )
        {
            elem_t elem = parse( interpreter.allocator(), in );

            try
            {
                std::string found = print( interpreter.process_operator( op, elem ) );
                fail( "Op "s + op + " failed for: '" + in + "'\nExpected error: '" + msg + "'\nFound:    '" + found + "'" );
            }
            catch( const Error<IllegalOp>& e )
            {
                test( e.message() == msg, "Op "s + op + " failed for: '" + in + "'\nExpected error: '" + msg + "'\nFound:          '" + e.message() + "'" );
                pass();
            }
        };

        test_op_illegal( '!', "21", "Illegal byte argument for !" );
        test_op( '!', "[0 21]", "[0 21]" );
    }

    static void run_tests()
    {
        std::cout << "TEST: " << TYPENAME( Env );

        test_interpreter();

        std::cout << "\n\n";
    }
};

void run_tests()
{
    Tester< Env< Debug, SimpleScheme, TestAllocator, KConInterpreter > >::run_tests();
    Tester< Env< Debug, SimpleScheme, SimpleAllocator, KConInterpreter > >::run_tests();
}

}	//namespace
