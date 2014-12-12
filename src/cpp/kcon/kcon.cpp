#include "tests.h"
#include <iostream>

int main( int argc, const char* argv[] )
{
	kcon::run_tests();
	std::cout << "** All tests passed **\n";
}