#include "elpa/console.h"
#include "kcon/interpreter.h"
#include "kcon/tests.h"

struct KConConsole : elpa::Console<kcon::KConInterpreter>
{
    virtual void run_tests() const
    {
        kcon::run_tests();
    }
};

auto main( int argc, const char* argv[] ) -> int
{
    return KConConsole().go( argc, argv );
}
