#include "tests.h"
#include "Allocator.h"
#include "stream.h"
#include "Parser.h"
#include <sstream>
#include <iostream>

void test_ostream()
{
	Allocator a;
	pcell_t p = new (a) Cell<value_t,pcell_t>{
					0,
					new (a) Cell<pcell_t,value_t>{
						new (a) Cell<value_t,value_t>{3,3},
						2 } };

	std::ostringstream os;
	KConOStream( os ) << "flat = " << p;
	KConOStream( os ) << "deep = " << deep << p;
	assert( os.str() == "flat = [0 [3 3] 2]deep = [0 [[3 3] 2]]", "Incorrect printing found '%s'", os.str().c_str()  );
}

void test_parser()
{
	Allocator a;
	std::istringstream iss("[0 [1 [2 3] 4] 5 6]");
	KConOStream( std::cout ) << Parser( a ).parse( iss );
}

void test_io( std::string in, KConOStream::Manip m, std::string out="" )
{
	if( out.empty() )
		out = in;
		
	Allocator a;
	std::istringstream iss( in );
	std::ostringstream oss;
	KConOStream( oss ) << m << Parser( a ).parse( iss );
	
	assert( oss.str() == out, _ << "IO failed for: " << in << "\nExpected: " << out << "\nFound: " << oss.str() );
}

void test_stream()
{
	test_io( "[0 0]", flat, "3" );
}

void run_tests()
{
	test_ostream();
	test_stream();
}