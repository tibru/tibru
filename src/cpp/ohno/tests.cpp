#include "tests.h"
#include "interpreter.h"
#include "../elpa/runtime.h"

using namespace elpa;

namespace ohno {

template<class Env>
struct Tester
{
    static void test_ohno()
    {TEST

    }

    static void run_tests()
    {
        std::cout << "TEST: " << TYPENAME( Env );

        test_ohno();

        std::cout << "\n\n";
    }
};

void run_tests()
{
    Tester< Env< Debug, SimpleScheme, TestAllocator, OhNoInterpreter > >::run_tests();
    Tester< Env< Debug, SimpleScheme, SimpleAllocator, OhNoInterpreter > >::run_tests();
}

}	//namespace
