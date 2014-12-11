#include "tests.h"
#include "Allocator.h"
#include "stream.h"
#include <sstream>

void test_ostream()
{
	Allocator a;
	pcell_t p = new (a) Cell<value_t,pcell_t>{
					0,
					new (a) Cell<pcell_t,value_t>{
						new (a) Cell<value_t,value_t>{3,3},
						2 } };

	std::ostringstream oss_flat;
	kostream( oss_flat ) << flat << p;
	auto expected_flat = "[0 [3 3] 2]";
	test( oss_flat.str() == expected_flat, "Incorrect flat printing found '" + oss_flat.str() + "'\nExpected '" + expected_flat + "'" );

	std::ostringstream oss_deep;
	kostream( oss_deep ) << deep << p;
	auto expected_deep = "[0 [[3 3] 2]]";
	test( oss_deep.str() == expected_deep, "Incorrect deep printing found '" + oss_deep.str() + "'\nExpected '" + expected_deep + "'" );
}

void test_io( const std::string& in, kostream::KManip m=flat, std::string out="" )
{
	if( out.empty() )
		out = in;

	Allocator a;
	std::istringstream iss( in );
	std::ostringstream oss;

    elem_t elem;
    kistream( iss, a ) >> elem;
    kostream( oss ) << m << elem;

	test( oss.str() == out, "IO failed for: '" + in + "'\nExpected: '" + out + "'\nFound:    '" + oss.str() + "'" );
}

void test_io_error( const std::string& in, const std::string& msg )
{
	Allocator a;
	std::istringstream iss( in );
	std::ostringstream oss;
	try
	{
	    elem_t elem;
	    kistream( iss, a ) >> elem;
        kostream( oss ) << elem;
	}
	catch( const Error<Syntax>& e )
	{
	    test( e.message() == msg, "IO failed for: '" + in + "'\nExpected error: '" + msg + "'\nFound:          '" + e.message() + "'" );
	    return;
	}

	test( false, "IO failed for: '" + in + "'\nExpected error: '" + msg + "'\nFound:    '" + oss.str() + "'" );
}

void test_stream()
{
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

	test_io_error( "[", "Unexpected end of input" );
	test_io_error( "]", "Unexpected ']'" );
	test_io_error( "[0]", "Unexpected singleton" );
	test_io_error( "[[0] 2]", "Unexpected singleton" );
	test_io_error( "[]", "Unexpected empty cell" );
	test_io_error( "[[] 2]", "Unexpected empty cell" );
}

void run_tests()
{
	test_stream();
	test_ostream();
}
