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

auto help( int ret_code ) -> int
{
    std::cout << "usage: kcon [option] <filenames>\n";
    std::cout << "Options are:\n";
    std::cout << "-mem=n : Limit memory usage to only n cells\n";
    std::cout << "-fast  : Turn off illegal operation checks and assertions\n";
    std::cout << "-safe  : Perfrom illegal operation checks only\n";
    std::cout << "-debug : Perfrom illegal operation checks and assertions\n";
    std::cout << "-repl  : Enter REPL after running all scripts\n";
    std::cout << "-tests : Run internal tests before starting\n";
    std::cout << "-help  : Show this help\n";
    std::cout << std::endl;
    return ret_code;
}

template<class Env>
auto run( size_t ncells, const std::vector< std::string >& filenames, bool repl ) -> int
{
	Shell< Env > shell( ncells );

    for( auto filename : filenames )
        shell.include( filename );

    if( repl )
        shell.interactive( std::cin, std::cout );

    return 0;
}

auto main( int argc, const char* argv[] ) -> int
{
    bool run_tests = false;
    size_t ncells = 0;
    std::string mode = "";
    bool repl = false;
    std::vector< std::string > filenames;

    for( const std::string arg : range( argv + 1, argv + argc ) )
    {
        if( arg.length() > 1 && arg[0] == '-' )
        {
            if( arg == "-tests" )
                run_tests = true;
            else if( arg.substr(0,5) == "-mem=" )
            {
                for( auto c : arg.substr(5) )
                {
                    if( !isdigit(c) )
                    {
                        std::cerr << "Invalid option: " << arg << std::endl;
                        return help( 1 );
                    }

                    ncells = 10 * ncells + (c - '0');
                }
            }
            else if( arg == "-debug" || arg == "-safe" || arg == "-fast" )
                mode = arg;
            else if( arg == "-repl" )
                repl = true;
            else if( arg == "-help" || arg == "--help" )
                return help( 0 );
            else
            {
                std::cerr << "Invalid option: " << arg << std::endl;
                return help( 1 );
            }
        }
        else
        {
            filenames.push_back( arg );
        }
    }

    if( ncells == 0 )
        ncells = 8 * 1024 * 1024;

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

    try
    {
        if( mode == "-debug" )
            return run< Env<Debug, SimpleScheme, TestAllocator, kcon::KConInterpreter> >( ncells, filenames, repl );
        else if( mode == "-safe" )
            return run< Env<Safe, OptScheme, OptAllocator, kcon::KConInterpreter> >( ncells, filenames, repl );
        else if( mode == "-fast" )
            return run< Env<Fast, OptScheme, OptAllocator, kcon::KConInterpreter> >( ncells, filenames, repl );
    }
    catch( const Error<Runtime,OutOfMemory>& )
    {
        std::cerr << "Out of memory" << std::endl;
        return 1;
    }

    std::cerr << "Unknown mode: "s + mode;
    return 1;
}
