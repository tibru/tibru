#include "kcon/console.h"

auto main( int argc, const char* argv[] ) -> int
{
    return kcon::KConConsole().go( argc, argv );
}
