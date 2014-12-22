#include "tests.h"
#include "runtime.h"
#include "shell.h"
#include <iostream>

using namespace kcon;

auto main( int argc, const char* argv[] ) -> int
{
	run_tests();
	std::cout << "\n** All tests passed **\n";

	Shell< Env<Debug, SimpleScheme, SimpleAllocator> > shell( std::cin, std::cout );

	shell.go();
}
