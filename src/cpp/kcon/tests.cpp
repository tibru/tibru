#include "tests.h"
#include "memory.h"
#include "runtime.h"
#include "stream.h"
#include <sstream>

namespace kcon {

template<class Env>
struct Tester
{
    typedef typename Env::Allocator Allocator;
    typedef typename Env::kostream kostream;
    typedef typename Env::kistream kistream;
    typedef typename Env::kostream::KManip KManip;

    static elem_t parse( Allocator& allocator, const std::string& in )
    {
        std::istringstream iss( in );
        elem_t elem = null<elem_t>();
        kistream( iss, allocator ) >> elem;
        return elem;
    }

    static void test_ostream()
    {TEST
        Allocator a( 1024 );

        pcell_t p = a.new_Cell(
                        1,
                        a.new_Cell(
                            a.new_Cell(3,3),
                            2 ) );

        std::ostringstream oss_flat;
        kostream( oss_flat ) << flat << p;
        auto expected_flat = "[1 [3 3] 2]";
        test( oss_flat.str() == expected_flat, "Incorrect flat printing found '" + oss_flat.str() + "'\nExpected '" + expected_flat + "'" );

        std::ostringstream oss_deep;
        kostream( oss_deep ) << deep << p;
        auto expected_deep = "[1 [[3 3] 2]]";
        test( oss_deep.str() == expected_deep, "Incorrect deep printing found '" + oss_deep.str() + "'\nExpected '" + expected_deep + "'" );
    }

    static void test_io( const std::string& in, KManip m=flat, std::string out="" )
    {
        if( out.empty() )
            out = in;

        Allocator a( 1024 );
        std::ostringstream oss;

        kostream( oss ) << m << parse( a, in );

        test( oss.str() == out, "IO failed for: '" + in + "'\nExpected: '" + out + "'\nFound:    '" + oss.str() + "'" );
    }

    template<class SubType=AnyType>
    static void test_io_error( const std::string& in, const std::string& msg )
    {
        Allocator a( 1024 );

        std::ostringstream oss;
        try
        {
            kostream( oss ) << parse( a, in );
        }
        catch( const Error<Syntax,SubType>& e )
        {
            test( e.message() == msg, "IO failed for: '" + in + "'\nExpected error: '" + msg + "'\nFound:          '" + e.message() + "'" );
            return pass();
        }

        test( false, "IO failed for: '" + in + "'\nExpected error: '" + msg + "'\nFound:    '" + oss.str() + "'" );
    }

    static void test_stream()
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
            pcell_t p = a.new_Cell( 1, 1 );
            a.new_Cell( 1, 1, {&p} );

            fail( "Failed to catch out of memory" );
        }
        catch( const Error<Runtime,OutOfMemory>& ) { pass(); }

        {
            Allocator a( 10 );
            pcell_t p = a.new_Cell( 1, 1 );
            a.new_Cell( 1, 1 );

            test( a.num_allocated() == 2, "Failed to register allocated cells" );

            a.gc({&p});

            test( a.num_allocated() == 1, "Failed to cleanup after GC" );
        }

        {
            Allocator a( 1024 );
            pcell_t p = parse( a, "[0 [1 [2 3] 4] 5 6]" ).pcell();
            pcell_t q = parse( a, "[0 [1 [2 3] 4] 5 6]" ).pcell();

            a.gc({&p,&q});

            test( a.gc_count() > 0, "GC failed to run with 2 roots" );
            test( a.num_allocated() == 12, "Failed to hold all cells in GC" );
        }

        {
            Allocator a( 1024 );
            pcell_t p = parse( a, "[0 [1 [2 3] 4] 5 6]" ).pcell();
            parse( a, "[0 [1 [2 3] 4] 5 6]" ).pcell();

            a.gc({&p});

            test( a.gc_count() > 0, "GC failed to run with 1 root" );
            test( a.num_allocated() == 6, "Failed to hold and cleanup all cells in GC" );
        }


        {
            //Test with minimal memory to create memory churn
            Allocator a( 1024 );
            pcell_t p = parse( a, "[0 [1 [2 3] 4] 5 6]" ).pcell();

            test( a.gc_count() == 0, "GC ran during parse" );
            //test( a.gc_count() == 1, "GC failed to run during parse" );

            a.gc({&p});

            test( a.num_allocated() == 6, "Failed to hold and cleanup all cells in GC" );
        }
    }

    static void run_tests()
    {
        std::cout << "TEST: " << TYPENAME( Env );

        test_stream();
        test_ostream();
        test_gc();
    }
};

void run_tests()
{
    Tester< Env<SimpleScheme, SimpleAllocator> >::run_tests();
}

}	//namespace
