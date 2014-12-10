#include "tests.h"
#include "Allocator.h"
#include "stream.h"
#include "Parser.h"
#include <sstream>

void test_ostream()
{
	Allocator a;
	pcell_t p = new (a) Cell<value_t,pcell_t>{
					0,
					new (a) Cell<pcell_t,value_t>{
						new (a) Cell<value_t,value_t>{3,3},
						2 } };

	std::ostringstream os;
	kostream( os ) << "flat = " << p;
	kostream( os ) << "deep = " << deep << p;
	test( os.str() == "flat = [0 [3 3] 2]deep = [0 [[3 3] 2]]", "Incorrect printing found '" + os.str() + "'" );
}

void test_io( const std::string& in, kostream::Manip m=flat, std::string out="" )
{
	if( out.empty() )
		out = in;

	Allocator a;
	std::istringstream iss( in );
	std::ostringstream oss;
	kostream( oss ) << m << Parser( a ).parse( iss );

	test( oss.str() == out, "IO failed for: '" + in + "'\nExpected: '" + out + "'\nFound:    '" + oss.str() + "'" );
}

void test_io_error( const std::string& in, const std::string& msg )
{
	Allocator a;
	std::istringstream iss( in );
	std::ostringstream oss;
	try
	{
        kostream( oss ) << Parser( a ).parse( iss );
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

	test_io_error( "[", "Unexpected end of input" );
	test_io_error( "]", "Unexpected ']'" );
	test_io_error( "[0]", "Unexpected singleton" );
	test_io_error( "[[0] 2]", "Unexpected singleton" );
	test_io_error( "[]", "Unexpected empty cell" );
	test_io_error( "[[] 2]", "Unexpected empty cell" );
}

void run_tests()
{
	test_ostream();
	test_stream();
}
