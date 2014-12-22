#include "elpa/tests.h"
#include "elpa/runtime.h"
#include "elpa/shell.h"
#include <iostream>

using namespace elpa;

auto main( int argc, const char* argv[] ) -> int
{
	elpa::run_tests();
	std::cout << "\n** All tests passed **\n";

	Shell< Env<Debug, SimpleScheme, SimpleAllocator> > shell( std::cin, std::cout );

	shell.go();
}
