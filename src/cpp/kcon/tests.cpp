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

    static auto parse( ShellManager& manager, const std::string& in ) -> auto_root_ref<elem_t>
    {
    	Defns defns( manager.interpreter().allocator() );
    	Readers readers = manager.readers();
    	Macros macros = manager.macros();
    	return parse( manager.interpreter().allocator(), in, defns, readers, macros );
    }

    static auto print( elem_t e ) -> std::string
    {
        std::ostringstream oss;
        elpa_ostream( oss ) << e;
        return oss.str();
    }

    static auto print( elem_t e, const Defns& defns ) -> std::string
    {
        std::ostringstream oss;
        elpa_ostream( oss ) << std::make_pair( &defns, e );
        return oss.str();
    }


    static void test_parse()
    {TEST
        ShellManager m( 1024 );
        test( print( parse( m, "#1000" ) ) == "[232 3 0 0]", "Incorrect parse of #" );

        try
        {
            parse( m, "#40000000000" );
            fail( "Parsed integer larger that 32bits with #" );
        }
        catch( Error<Syntax> ) { pass(); }

        auto test_parse = [&]( const std::string& in, const std::string& out )
        {
            elem_t elem = parse( m, in );

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
        test_parse( "[1' 2' 3' < 4']", "[[0 3] [0 2] [0 1] 0 4]" );
    }

    static void test_shell()
    {TEST
        Shell< Env > shell( 100 );

        shell.parse( ":names on" );
        shell.parse( ":def x 0" );

        auto test_parse = [&]( const std::string& in, const std::string& out )
        {
            std::string found = print( shell.parse( in ), shell.names() );

            test( found == out, "Parse failed for: '" + in + "'\nExpected: '" + out + "'\nFound:    '" + found + "'" );
        };

        test_parse( "[1' 2' 3' x <]", "[x [qt 3] [qt 2] qt 1]" );
    }

    static void test_operators()
    {TEST
        Shell< Env > shell( 100 );

        shell.parse( ":names on" );

        auto test_op = [&]( const std::string& in, const std::string& out )
        {
            std::string found = print( shell.parse( in ), shell.names() );

            test( found == out, "Op failed for: '" + in + "'\nExpected: '" + out + "'\nFound:    '" + found + "'" );
        };

        auto test_op_illegal = [&]( const std::string& in, const std::string& msg )
        {
            try
            {
                std::string found = print( shell.parse( in ) );
                fail( "Op failed for: '" + in + "'\nExpected error: '" + msg + "'\nFound:    '" + found + "'" );
            }
            catch( const Error<IllegalOp>& e )
            {
                test( e.message() == msg, "Op failed for: '" + in +" '\nExpected error: '" + msg + "'\nFound:          '" + e.message() + "'" );
                pass();
            }
        };

        //Constant
        test_op( ".[0 21]", "[0 21]" );

        //Select
        test_op( "/[[1 2 3] [0 0] 0]", "[1 2 3]");
        test_op( "/[[1 2 3] [1 0] 0]", "[2 3]");
        test_op( "/[[1 2 3] [0 0] 1]", "1");
        test_op( "/[[1 2 3 4 5] #2 0]", "[3 4 5]");
        test_op( "/[[1 2 3 4 5] #2 1]", "3");
        test_op( "/[[1 2 3 4 5] #1 0 #1 0]", "[3 4 5]");
        test_op( "/[[1 2 3 4 5] #1 0 #2 0]", "[4 5]");
        test_op( "/[[[[1 2] [3 4]] [[5 6] [7 8]]] #1 1]", "[5 6]");
        test_op( "/[[[[1 2] [3 4]] [[5 6] [7 8]]] #1 1 #1 0]", "6");
        test_op( "/[[[0 1 2 3 nil] [1 2 3 4 nil] [2 3 4 5 nil] nil] #1 1 #2 1]", "3");
        test_op( "/[[[0 1 2 3 nil] [1 2 3 4 nil] [2 3 4 5 nil] nil] #1 1 #3 1]", "4");
        test_op( "/[[[0 1 2 3 nil] [1 2 3 4 nil] [2 3 4 5 nil] nil] #2 1 #2 1]", "4");
        test_op( "/[[[0 1 2 3 nil] [1 2 3 4 nil] [2 3 4 5 nil] nil] #0 1 #0 1]", "0");

        test_op_illegal( "/0", "/ operates only on cells" );
        test_op_illegal( "/[[1 2 3] 0]", "/ requires paths of the form [([] b)+]" );
        test_op_illegal( "/[[1 2 3] [0 0]]", "Path tails count must be cells" );
        test_op_illegal( "/[[1 2 3] [0 0] [[0 0] [0 0]]]", "Path head count must be a byte" );
        test_op_illegal( "/[[1 2 3] [0 0] 2 3]", "Path tail count must not be a byte" );
        test_op_illegal( "/[[1 2 3] [0 0] 0 [0 0]]", "Path tails count must be cells" );
        test_op_illegal( "/[[1 2 3] [255 255 255 255 255 255 255 255 255 255] 4]", "Path tail count overflow" );
        test_op_illegal( "/[[1 2 3] #4 0]", "Tried to access tail of a byte" );
        test_op_illegal( "/[[1 2 3] #0 4]", "Tried to access head of a byte" );

        //IfCell
        test_op( "?[[0 1] [0 0]]", "1" );
        test_op( "?[[0 1] 0]", "0" );
        test_op_illegal( "?0", "? operates only on cells" );

        //Graft
        test_op( "+[0 21 [#0 0]]", "21" );
        test_op( "+[[0 1] 21 [#1 0]]", "[0 21]" );
        test_op( "+[[0 1 2] [21 22] [#2 0]]", "[0 1 21 22]" );
        test_op( "+[[0 1 2] [21 22] [#0 1]]", "[[21 22] 1 2]" );
        test_op( "+[[[0 1] [2 3]] [21 22] [#1 1]]", "[[0 1] [21 22] 3]" );
        test_op( "+[[[[1 2] [3 4]] [[5 6] [7 8]]] 21 #1 1 #1 0]", "[[[1 2] 3 4] [5 21] 7 8]" );
        test_op( "+[[[[1 2] [3 4]] [[5 6] [7 8]]] 21 #1 1 #0 1]", "[[[1 2] 3 4] [21 6] 7 8]" );
        test_op_illegal( "+[0 0]", "+ requires path and element" );
        test_op_illegal( "+[0 0 0]", "+ requires path to be a cell" );
        test_op_illegal( "+[0 0 [0 0]]", "Path tails count must be cells" );
        test_op_illegal( "+[0 21 [#1 0]]", "Tried to access tail of a byte" );
        test_op_illegal( "+[0 21 [#0 1]]", "Tried to access head of a byte" );

        //Reduce
        test_op( "@[nil qt 21]", "21" );
        test_op( "@[[10 20 30] sel #1 1]", "20" );
        test_op( "@[[10 20 30] sel #1 0 #1 0]", "30" );
        test_op( "@[[10 20 30] if [0 1] T]", "1" );
        test_op( "@[[10 20 30] if [0 1] F]", "0" );
        test_op_illegal( "@21", "@ operates only on cells" );
        test_op_illegal( "@[2 3]", "@ requires cell expression" );
        test_op_illegal( "@[nil 3 0]", "@ requires expression code to be 0 or 1" );
        test_op_illegal( "@[[10 20 30] if 0]", "@ 2 requires cell based choices and condition" );
        test_op_illegal( "@[[10 20 30] if 0 0]", "@ 2 requires cell based choices" );

        //Evaluate
        test_op( "*[nil qt 21]", "21" );
        test_op( "*[[10 20 30] sel #1 1]", "20" );
        test_op( "*[[10 20 30] sel #1 0 #1 0]", "30" );
        test_op( "*[EXIT [[qt 2] [qt 3]]]", "[3 2]" );
        test_op( "*[EXIT [[qt 1] [qt 2] [qt 3]]]", "[3 2 1]" );
        test_op( "*[EXIT [[qt 1] [qt 2] [qt 3] [qt 4]]]", "[4 3 2 1]" );
        test_op( "*[EXIT 1' 2' 3' 4']", "[4 3 2 1]" );
        test_op( "*[[0 10 20 30 40 50 nil] [[sel #1 1] [sel #2 1] [sel #3 1] [sel #4 1]]]", "[40 30 20 10]" );
        test_op( "*[nil [if [0 1] 0] [if [0 1] [0 0]]]", "[1 0]" );
        test_op_illegal( "*21", "* operates only on cells" );
        test_op_illegal( "*[2 3]", "* requires cell expression" );
        test_op_illegal( "*[nil 3 0]", "@ requires expression code to be 0 or 1" );
        test_op_illegal( "*[EXIT [[qt 2] 0]]", "* cons form requires at least 2 cell based expressions" );

        shell.parse( ":def env [1 #0 0]" );
        shell.parse( ":def EXITENV [env EXIT']" );

        //Execute
        test_op( "![EXIT 21]", "21" );
        test_op( "![EXIT 21']", "[qt 21]" );
        test_op( "![[env 22' EXITENV'] 23]", "[22 23]" );
        test_op( "![GRAFT [1 2 3] [EXIT 21'] [#0 0]]", "[qt 21]" );
        test_op( "![GRAFT [EXIT 21] 22 [#1 0]]", "22" );
    }

    static void run_tests()
    {
        std::cout << "TEST: " << TYPENAME( Env );

        test_parse();
        test_shell();
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
