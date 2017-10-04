#include "tests.h"
#include "memory.h"
#include "runtime.h"
#include "stream.h"
#include "container/range.h"
#include "shell.h"
#include <sstream>

namespace elpa {

using container::valrange;

template<class Env>
struct Tester
{
    typedef typename Env::Scheme Scheme;
    typedef typename Env::Allocator Allocator;
    typedef typename Env::elpa_ostream elpa_ostream;
    typedef typename Env::elpa_istream elpa_istream;
    typedef typename elpa_istream::Defns Defns;
    typedef typename elpa_istream::Readers Readers;
    typedef typename elpa_istream::Macros Macros;
    typedef typename Env::elpa_ostream::ElpaManip ElpaManip;

    typedef typename Scheme::pcell_t pcell_t;
    typedef typename Scheme::elem_t elem_t;
    typedef typename Scheme::byte_t byte_t;

    template<class T>
    using auto_root_ref = typename Allocator::template auto_root_ref<T>;

    template<class T>
    using auto_root = typename Allocator::template auto_root<T>;

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
    	Readers readers;
    	Macros macros;
    	return parse( allocator, in, defns, readers, macros );
    }

    static auto print( elem_t e, const Defns& defns, ElpaManip m=flat ) -> std::string
    {
        std::ostringstream oss;
        elpa_ostream( oss ) << m << std::make_pair( &defns, e );
        return oss.str();
    }

    static void test_ostream()
    {TEST
        Allocator a( 1024 );

        Defns defns( a );

        auto_root<elem_t> p( a, a.new_Cell(
                        byte_t(1),
                        a.new_Cell(
                            a.new_Cell( byte_t(3), byte_t(3) ),
                            byte_t(2) ) ) );

        auto found_flat = print( p, defns, flat );
        auto expected_flat = "[1 [3 3] 2]";
        test( found_flat == expected_flat, "Incorrect flat printing found '" + found_flat + "'\nExpected '" + expected_flat + "'" );

        auto found_deep = print( p, defns, deep );
        auto expected_deep = "[1 [[3 3] 2]]";
        test( found_deep == expected_deep, "Incorrect deep printing found '" + found_deep + "'\nExpected '" + expected_deep + "'" );
    }

    static void test_istream()
    {TEST
    	Allocator alloc( 1024 );

        Defns defns( alloc );
        Readers readers = {
            {'$', []( Allocator& a, std::istream& is ) -> elem_t {
                uint8_t l = 0;
                char c;
                while( is.get( c ) && isdigit(c) )
                    ++l;
                if( !isdigit(c) )
                    is.putback( c );
                return byte_t( l );
            } }
        };

        Macros macros = {
            {'%', []( Allocator& a, elem_t tail, std::vector<std::string>& names ) -> elem_t {
                return a.new_Cell( tail.pcell()->head(), tail );
            } },
            {'+', []( Allocator& a, elem_t tail, std::vector<std::string>& names ) -> elem_t {
                return a.new_Cell( byte_t( Scheme::byte_value( tail.pcell()->head().byte() ) + 1 ), tail.pcell()->tail() );
            } }
        };

        auto parse = [&]( std::string s ) { return Tester::parse( alloc, s, defns, readers, macros ); };
        defns["x"] = parse( "0" );
        defns["y"] = parse( "[x 1 x]" );
        defns["z"] = parse( "[y 2 x]" );
        defns["l"] = parse( "$123" );

        auto test_i = [&]( std::string in, std::string out, std::string named_out="" )
        {
        	auto r = print( parse( in ), Defns(alloc) );
        	test( r == out, "Unamed parse of '"s + in + "' incorrect.\nExpected " + out + "\nFound: " + r );

            if( named_out != "" )
        	{
        	    auto s = print( parse( in ), defns );
                test( s == named_out, "Named parse of '"s + in + "' incorrect.\nExpected " + named_out + "\nFound: " + s );
        	}
        };

        try { test_i( "notfound", "", "" ); fail( "Parsed undefined element" ); }
        catch( Error<Syntax,Undef> ) { pass(); }

        try { test_i( "[notfound 0]", "", "" ); fail( "Parsed undefined element" ); }
        catch( Error<Syntax,Undef> ) { pass(); }

    	test_i( "0", "0" );
    	test_i( "1", "1" );
    	test_i( "x", "0" );
    	test_i( "y", "[0 1 0]" );
    	test_i( "z", "[[0 1 0] 2 0]" );
    	test_i( "[x y z]", "[0 [0 1 0] [0 1 0] 2 0]", "[x y z]" );
    	test_i( "[x 21 z]", "[0 21 [0 1 0] 2 0]", "[x 21 z]" );
    	test_i( "[l $3456]", "[3 4]", "[l 4]" );
    	test_i( "[4 [5 6] %]", "[4 [5 6] 5 6]", "[4 [5 6] 5 6]" );
    	test_i( "[4 4 +]", "[4 5]", "[4 5]" );
    	test_i( "[4 4+]", "[4 5]", "[4 5]" );
    	test_i( "4 +", "5" );
    	test_i( "4++", "6" );
    	test_i( "y <dontparsethis>", "[0 1 0]" );

    	try { test_i( "%", "" ); fail( "Used macro with no expression" ); }
    	catch( Error<Syntax> e ) { test( e.message() == "Unexpected macro '%'", "Found: "s + e.message() ); }
    }

    static void test_io( const std::string& in, ElpaManip m=flat, std::string out="" )
    {
        if( out.empty() )
            out = in;

        Allocator a( 1024 );
        Defns defns( a );
        std::ostringstream oss;

        auto found = print( parse( a, in ), defns, m );

        test( found == out, "IO failed for: '" + in + "'\nExpected: '" + out + "'\nFound:    '" + found + "'" );
    }

    template<class SubType=AnyType>
    static void test_io_error( const std::string& in, const std::string& msg )
    {
        Allocator a( 1024 );
        Defns defns( a );

        std::string found;
        try
        {
            found = print( parse( a, in ), defns );
            fail( "IO failed for: '" + in + "'\nExpected error: '" + msg + "'\nFound:    '" + found + "'" );
        }
        catch( const Error<Syntax,SubType>& e )
        {
            test( e.message() == msg, "IO failed for: '" + in + "'\nExpected error: '" + msg + "'\nFound:          '" + e.message() + "'" );
            pass();
        }
    }

    static void test_iostream()
    {TEST
        test_io( "3" );
        test_io( " 3 ", flat, "3" );
        test_io( "[0 [1 2]]", flat, "[0 1 2]" );
        test_io( " [ 0 [ 1\n 2]\t]\t", flat, "[0 1 2]" );
        test_io( "[0 1 2]", deep, "[0 [1 2]]" );
        test_io( "[0 [1 [2 3] 4] 5 6]", flat );
        test_io( "[0 [1 [2 3] 4] 5 6]", deep, "[0 [[1 [[2 3] 4]] [5 6]]]" );
        test_io( "[[0 1] 2]", deep );
        test_io( "[[0 1] 2]", flat );
        test_io( "[[[0 1] [2 3]] [[4 5] [6 7]]]", deep );
        test_io( "[[[0 1] [2 3]] [[4 5] [6 7]]]", flat, "[[[0 1] 2 3] [4 5] 6 7]" );
        test_io( "[[[0 1] 2 3] [4 5] 6 7]", flat );
        test_io( "[0 1] <dontparsethis>", flat, "[0 1]" );
        test_io( "0 <dontparsethis>", flat, "0" );

        test_io_error<EOS>( "", "Unexpected end of input" );
        test_io_error<EOS>( "[", "Unexpected end of input" );
        test_io_error<EOS>( "[45 ", "Unexpected end of input (unclosed pair)" );
        test_io_error<EOS>( "[45 34", "Unexpected end of input (unclosed pair)" );
        test_io_error<EOS>( "[45 [34", "Unexpected end of input (unclosed pair)" );
        test_io_error<Undef>( "ref", "Undefined reference to 'ref'" );
        test_io_error( "]", "Unexpected ']'" );
        test_io_error( "[0]", "Unexpected singleton" );
        test_io_error( "[[0] 2]", "Unexpected singleton" );
        test_io_error( "[]", "Unexpected empty cell" );
        test_io_error( "[[] 2]", "Unexpected empty cell" );
    }

    static void test_gc()
    {TEST
        try
        {
            Allocator a( 1 );
            auto_root<elem_t> p( a, a.new_Cell( byte_t(1), byte_t(1) ) );
            a.new_Cell( byte_t(1), byte_t(1) );

            fail( "Failed to catch out of memory" );
        }
        catch( const Error<Runtime,OutOfMemory>& ) { pass(); }

        {
            Allocator a( 10 );
            auto_root<elem_t> p( a, a.new_Cell( byte_t(1), byte_t(2) ) );
            a.new_Cell( byte_t(1), byte_t(1) );

            test( a.num_allocated() == 2, "Failed to register allocated cells" );

            test( p->head().is_byte() && Scheme::byte_value( p->head().byte() ) == 1, "Cell head incorrect before GC" );
            test( p->tail().is_byte() && Scheme::byte_value( p->tail().byte() ) == 2, "Cell tail incorrect before GC" );

            a.gc();

            test( p->head().is_byte() && Scheme::byte_value( p->head().byte() ) == 1, "Cell head incorrect after GC" );
            test( p->tail().is_byte() && Scheme::byte_value( p->tail().byte() ) == 2, "Cell tail incorrect after GC" );

            test( a.num_allocated() == 1, "Failed to cleanup after GC" );
        }

        {
            Allocator a( 1024 );
            Defns defns( a );

            auto_root<elem_t> p( parse( a, "[0 [1 [2 3] 4] 5 6]" ) );
            auto_root<elem_t> q( parse( a, "[0 [1 [2 3] 4] 5 6]" ) );

            a.gc();

            test( a.gc_count() > 0, "GC failed to run with 2 roots" );
            test( a.num_allocated() == 12, "Failed to hold all cells in GC" );

            test( print( p, defns ) == "[0 [1 [2 3] 4] 5 6]", "Complex tree (1) altered by GC" );
            test( print( q, defns ) == "[0 [1 [2 3] 4] 5 6]", "Complex tree (2) altered by GC" );
        }

        {
            Allocator a( 1024 );
            Defns defns( a );

            elem_t pp = parse( a, "[0 [1 [2 3] 4] 5 6]" );
            {
            	auto_root<elem_t> p( a, pp );
            	parse( a, "[0 [1 [2 3] 4] 5 6]" );

            	a.gc();

	            test( a.gc_count() > 0, "GC failed to run with 1 root" );
                test( a.num_allocated() == 6, "Failed to hold and cleanup all cells in GC" );

	            test( print( p, defns ) == "[0 [1 [2 3] 4] 5 6]", "Complex tree (3) altered by GC" );
	            pp = p;
            }

            auto_root<elem_t> t( a, pp->tail() );

            a.gc();

            test( a.num_allocated() == 5, "Failed to hold and cleanup tail cells in GC" );
            test( print( t, defns ) == "[[1 [2 3] 4] 5 6]", "Complex tree tail altered by GC" );
        }

        {
            //Test with minimal memory to create memory churn
            for( auto n : valrange(1,13) )
            {
                try
                {
                    Allocator a( n );
                    parse( a, "[0 [1 [2 3] 4] 5 6]" );
                    fail( "Parsed in low memory enviroment" );
                }
                catch( const Error<Runtime,OutOfMemory>& )
                {
                    pass();
                }
            }
        }

        {
            //Test with minimal memory to create memory churn
            for( auto n : valrange(13,20) )
            {
                try
                {
                    Allocator a( n );
                    Defns defns( a );

                    auto_root<elem_t> p( parse( a, "[0 [1 [2 3] 4] 5 6]" ) );

                    test( a.gc_count() > 0, "GC failed to run during low memory parse" );

                    a.gc();

                    test( print( p, defns ) == "[0 [1 [2 3] 4] 5 6]", "Low memory parse tree tail altered by GC" );
                    test( a.num_allocated() == 6, "Failed to hold and cleanup all cells in GC" );
                }
                catch( const Error<Runtime,OutOfMemory>& )
                {
                    fail( "Ran out of memory in minimal memory test, increase test threshold to fix the test" );
                }
            }
        }
    }

    static void test_shell()
    {TEST
    	Shell< Env > shell( 1024 );

    	test( shell.parse( "" ).is_undef(), "Blank script doesn't process to undefined" );
		test( print( shell.parse( "8" ), shell.names() ) == "8", "Byte doesn't process to itself" );
		test( print( shell.parse( "[8 9]" ), shell.names() ) == "[8 9]", "Pair doesn't process to itself" );
		test( print( shell.parse( "[8 9]\n[0 1]" ), shell.names() ) == "[0 1]", "2 expressions don't process to last" );
		test( print( shell.parse( "[8 9]\n[it 8]" ), shell.names() ) == "[[8 9] 8]", "Use of 'it' failed" );
		test( print( shell.parse( ":def t [1 2] \n"
                                  "\t0 \n"
                                  ":gc\n"
                                  ":sys\n"	//check noisiness
                                  ":def h it\n"
                                  "[h t]" ), shell.names() ) == "[0 1 2]", "Complex shell script failed" );

		test( print( shell.parse( "t" ), shell.names() ) == "[1 2]", "Failed to hold reference between processes (1)" );
		test( print( shell.parse( "[t t]" ), shell.names() ) == "[[1 2] 1 2]", "Failed to hold reference between processes (2)" );
		test( print( shell.parse( "[t t it]" ), shell.names() ) == "[[1 2] [1 2] [1 2] 1 2]", "Failed to hold reference between processes (3)" );

		shell.parse( ":names on" );

		test( print( shell.parse( ":def t [1 2] \n"
                                  "\t0 \n"
                                  ":gc\n"
                                  ":sys\n"	//check noisiness
                                  ":def h it\n"
                                  "[h t]" ), shell.names() ) == "[h t]", "Complex shell script failed (with names)" );

		test( print( shell.parse( "t" ), shell.names() ) == "[1 2]", "Failed to hold reference between processes (1) (with names)" );
		test( print( shell.parse( "[t t]" ), shell.names() ) == "[t t]", "Failed to hold reference between processes (2) (with names)" );
		test( print( shell.parse( "[t t it]" ), shell.names() ) == "[t t t t]", "Failed to hold reference between processes (3) (with names)" );

		try { shell.parse( "z" ); fail( "Shell returned undefined reference (1)" ); }
		catch( Error<Syntax,Undef> ) { pass(); }

		try { shell.parse( "[z z]" ); fail( "Shell returned undefined reference (2)" ); }
		catch( Error<Syntax,Undef> ) { pass(); }
    }

    static void run_tests()
    {
        std::cout << "TEST: " << TYPENAME( Env );

        test_ostream();
        test_istream();
        test_iostream();
        test_gc();
        test_shell();

        std::cout << "\n\n";
    }
};

void run_tests()
{
    Tester< Env< Debug, SimpleScheme, TestAllocator, NullInterpreter > >::run_tests();
    Tester< Env< Debug, SimpleScheme, SimpleAllocator, NullInterpreter > >::run_tests();
}

}	//namespace
