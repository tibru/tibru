#include "shell.h"
#include "runtime.h"
#include <sstream>

using namespace kcon;

template<class Env>
void Shell<Env>::go()
{
    while( true )
    {
        std::string input;
        elem_t elem;

        std::string prompt = ">>> ";
        while( true )
        {
            try
            {
                _out << prompt;
                std::string line;
                std::getline( _in, line );
                input += (line + "\n"s);
                prompt = "... ";

                std::istringstream iss( input );


                kistream( iss, _alloc ) >> elem;

                char c;
                while( iss >> c )
                    if( !isspace( c ) )
                        throw Error<Syntax>( "unexpected character after expression '"s + c + "'" );

                break;
            }
            catch( const Error<Syntax,EOS>& )
            {
            }
        }

        kostream( _out ) << elem << std::endl;
    }
}

template class Shell< Env<Debug, SimpleScheme, SimpleAllocator> >;
