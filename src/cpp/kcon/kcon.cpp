#include "common.h"
#include "types.h"
#include "stream.h"
#include "Parser.h"
#include <iostream>
#include <sstream>

void test_ostream()
{
	Allocator a;
	pcell_t p = new (a) Node<value_t,pcell_t>{
					0,
					new (a) Node<pcell_t,value_t>{
						new (a) Node<value_t,value_t>{3,3},
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

int main( int argc, const char* argv[] )
{
	test_ostream();
	test_parser();
	std::cout << "** All tests passed **\n";
}
