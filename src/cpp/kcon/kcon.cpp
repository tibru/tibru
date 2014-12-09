#include "tests.h"
#include <iostream>

int main( int argc, const char* argv[] )
{
	test_ostream();
	test_parser();
	std::cout << "** All tests passed **\n";
}
