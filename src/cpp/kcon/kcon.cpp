#include "tests.h"
#include <iostream>

auto main( int argc, const char* argv[] ) -> int
{
	kcon::run_tests();
	std::cout << "\n** All tests passed **\n";
}
