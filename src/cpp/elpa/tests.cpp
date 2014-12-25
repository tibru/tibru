#include "tests.h"
#include "memory.h"
#include "runtime.h"
#include "stream.h"
#include "container/range.h"
#include <sstream>

namespace elpa {

using container::range;

template<class Env>
struct Tester
{
    typedef typename Env::Allocator Allocator;
    typedef typename Env::elpa_ostream elpa_ostream;
    typedef typename Env::elpa_istream elpa_istream;
    typedef typename elpa_istream::Names Names;
    typedef typename Env::elpa_ostream::ElpaManip ElpaManip;

    typedef typename Env::Scheme::pcell_t pcell_t;
    typedef typename Env::Scheme::elem_t elem_t;

    template<class T>
    using auto_root_ref = typename Allocator::template auto_root_ref<T>;

    template<class T>
    using auto_root = typename Allocator::template auto_root<T>;

    static auto parse( Allocator& allocator, const std::string& in, const Names& names ) -> auto_root_ref<elem_t>
    {
        std::istringstream iss( in );
        elem_t elem;
        elpa_istream( iss, allocator, names ) >> elem;
        return auto_root_ref<elem_t>( allocator, elem );
    }
    
    static auto parse( Allocator& allocator, const std::string& in ) -> auto_root_ref<elem_t>
    {
    	Names names( allocator );
    	return parse( allocator, in, names );
    }

    static auto print( elem_t e, ElpaManip m=flat ) -> std::string
    {
        std::ostringstream oss;
        elpa_ostream( oss ) << m << e;
        return oss.str();
    }

    static void test_ostream()
    {TEST
        Allocator a( 1024 );

        auto_root<elem_t> p( a, a.new_Cell(
                        1,
                        a.new_Cell(
                            a.new_Cell( 3, 3 ),
                            2 ) ) );

        auto found_flat = print( p, flat );
        auto expected_flat = "[1 [3 3] 2]";
        test( found_flat == expected_flat, "Incorrect flat printing found '" + found_flat + "'\nExpected '" + expected_flat + "'" );

        auto found_deep = print( p, deep );
        auto expected_deep = "[1 [[3 3] 2]]";
        test( found_deep == expected_deep, "Incorrect deep printing found '" + found_deep + "'\nExpected '" + expected_deep + "'" );
    }
    
    static void test_istream()
    {TEST
    	Allocator a( 1024 );
    	
        Names names( a );
        names["x"] = parse( a, "0", names );
        names["y"] = parse( a, "[x 1 x]", names );
        names["z"] = parse( a, "[y 2 x]", names );
        
        auto test_i = [&a, &names]( std::string in, std::string out )
        {
        	auto r = print( parse( a, in, names ) );
        	test( r == out, "Named parse of '"s + in + "' incorrect.\nExpected " + out + "\nFound: " + r );
        };
        
        try { test_i( "notfound", "" ); fail( "Parsed undefined element" ); }
        catch( Error<Syntax,Undef> ) { pass(); }

        try { test_i( "[notfound 0]", "" ); fail( "Parsed undefined element" ); }
        catch( Error<Syntax,Undef> ) { pass(); }
                   
    	test_i( "0", "0" );   
    	test_i( "x", "0" );
    	test_i( "y", "[0 1 0]" );
    	test_i( "z", "[[0 1 0] 2 0]" );
    	test_i( "[x y z]", "[0 [0 1 0] [0 1 0] 2 0]" );
    }

    static void test_io( const std::string& in, ElpaManip m=flat, std::string out="" )
    {
        if( out.empty() )
            out = in;

        Allocator a( 1024 );
        std::ostringstream oss;

        auto found = print( parse( a, in ), m );

        test( found == out, "IO failed for: '" + in + "'\nExpected: '" + out + "'\nFound:    '" + found + "'" );
    }

    template<class SubType=AnyType>
    static void test_io_error( const std::string& in, const std::string& msg )
    {
        Allocator a( 1024 );

        std::string found;
        try
        {
            found = print( parse( a, in ) );
        }
        catch( const Error<Syntax,SubType>& e )
        {
            test( e.message() == msg, "IO failed for: '" + in + "'\nExpected error: '" + msg + "'\nFound:          '" + e.message() + "'" );
            return pass();
        }

        test( false, "IO failed for: '" + in + "'\nExpected error: '" + msg + "'\nFound:    '" + found + "'" );
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

        test_io_error<EOS>( "[", "Unexpected end of input" );
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
            auto_root<elem_t> p( a, a.new_Cell( 1, 1 ) );
            a.new_Cell( 1, 1 );

            fail( "Failed to catch out of memory" );
        }
        catch( const Error<Runtime,OutOfMemory>& ) { pass(); }

        {
            Allocator a( 10 );
            auto_root<elem_t> p( a, a.new_Cell( 1, 2 ) );
            a.new_Cell( 1, 1 );

            test( a.num_allocated() == 2, "Failed to register allocated cells" );

            test( p->head().is_byte() && p->head().byte() == 1, "Cell head incorrect before GC" );
            test( p->tail().is_byte() && p->tail().byte() == 2, "Cell tail incorrect before GC" );

            a.gc();

            test( p->head().is_byte() && p->head().byte() == 1, "Cell head incorrect after GC" );
            test( p->tail().is_byte() && p->tail().byte() == 2, "Cell tail incorrect after GC" );

            test( a.num_allocated() == 1, "Failed to cleanup after GC" );
        }

        {
            Allocator a( 1024 );
            auto_root<elem_t> p( parse( a, "[0 [1 [2 3] 4] 5 6]" ) );
            auto_root<elem_t> q( parse( a, "[0 [1 [2 3] 4] 5 6]" ) );

            a.gc();

            test( a.gc_count() > 0, "GC failed to run with 2 roots" );
            test( a.num_allocated() == 12, "Failed to hold all cells in GC" );

            test( print( p ) == "[0 [1 [2 3] 4] 5 6]", "Complex tree (1) altered by GC" );
            test( print( q ) == "[0 [1 [2 3] 4] 5 6]", "Complex tree (2) altered by GC" );
        }

        {
            Allocator a( 1024 );
            elem_t pp = parse( a, "[0 [1 [2 3] 4] 5 6]" );
            {
            	auto_root<elem_t> p( a, pp );
            	parse( a, "[0 [1 [2 3] 4] 5 6]" );

            	a.gc();

	            test( a.gc_count() > 0, "GC failed to run with 1 root" );
 	           test( a.num_allocated() == 6, "Failed to hold and cleanup all cells in GC" );

	            test( print( p ) == "[0 [1 [2 3] 4] 5 6]", "Complex tree (3) altered by GC" );
	            pp = p;
            }

            auto_root<elem_t> t( a, pp->tail() );

            a.gc();

            test( a.num_allocated() == 5, "Failed to hold and cleanup tail cells in GC" );
            test( print( t ) == "[[1 [2 3] 4] 5 6]", "Complex tree tail altered by GC" );
        }

        {
            //Test with minimal memory to create memory churn
            for( auto n : range(1,11) )
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
            for( auto n : range(11,20) )
            {
                Allocator a( n );
                auto_root<elem_t> p( parse( a, "[0 [1 [2 3] 4] 5 6]" ) );

                test( a.gc_count() > 0, "GC failed to run during low memory parse" );

                a.gc();

                test( print( p ) == "[0 [1 [2 3] 4] 5 6]", "Low memory parse tree tail altered by GC" );
                test( a.num_allocated() == 6, "Failed to hold and cleanup all cells in GC" );
            }
        }
    }

    static void run_tests()
    {
        std::cout << "TEST: " << TYPENAME( Env );

        test_ostream();
        test_istream();
        test_iostream();
        test_gc();

        std::cout << "\n\n";
    }
};

void run_tests()
{
    Tester< Env< Debug, SimpleScheme, TestAllocator, NullInterpreter > >::run_tests();
    Tester< Env< Debug, SimpleScheme, SimpleAllocator, NullInterpreter > >::run_tests();
}

}	//namespace