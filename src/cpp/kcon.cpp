#include "elpa/tests.h"
#include "kcon/tests.h"
#include "elpa/runtime.h"
#include "elpa/shell.h"
#include "kcon/interpreter.h"
#include "kcon/shell.h"
#include "elpa/container/range.h"
#include <iostream>

using namespace elpa;
using namespace elpa::container;

auto info( int ret_code ) -> int
{
    std::cout << "KCon" << std::endl;
    return ret_code;
}

template<class Env>
auto run( const std::vector< std::string >& filenames, bool repl ) -> int
{
	Shell< Env > shell;

    for( auto filename : filenames )
        shell.include( filename );

    if( repl )
        shell.interactive( std::cin, std::cout );

    return 0;
}

auto main( int argc, const char* argv[] ) -> int
{
    bool run_tests = false;
    std::string mode = "";
    bool repl = false;
    std::vector< std::string > filenames;

    for( const std::string arg : range( argv + 1, argv + argc ) )
    {
        if( arg.length() > 1 && arg[0] == '-' )
        {
            if( arg == "-tests" )
                run_tests = true;
            else if( arg == "-debug" || arg == "-safe" || arg == "-fast" )
                mode = arg;
            else if( arg == "-repl" )
                repl = true;
            else if( arg == "-help" || arg == "--help" )
                return info( 0 );
            else
            {
                std::cerr << "Invalid option: " << arg << std::endl;
                return info( 1 );
            }
        }
        else
        {
            filenames.push_back( arg );
        }
    }

    if( mode == "" )
        mode = "-debug";

    if( filenames.size() == 0 )
        repl = true;

    if( run_tests )
    {
        try
        {
            elpa::run_tests();
            kcon::run_tests();
            std::cout << "\n** All tests passed **\n";
        }
        catch( const Error<Test>& e )
        {
            std::cerr << e.message() << std::endl;
            return 2;
        }
    }

    if( mode == "-debug" )
        return run< Env<Debug, SimpleScheme, TestAllocator, kcon::KConInterpreter> >( filenames, repl );
    else if( mode == "-safe" )
        return run< Env<Safe, OptScheme, OptAllocator, kcon::KConInterpreter> >( filenames, repl );
    else if( mode == "-fast" )
        return run< Env<Fast, OptScheme, OptAllocator, kcon::KConInterpreter> >( filenames, repl );

    std::cerr << "Unknown mode: "s + mode;
    return 1;
}
