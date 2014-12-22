#include "shell.h"
#include "runtime.h"
#include <sstream>

using namespace elpa;

template<class Env>
void Shell<Env>::read_command( elpa_istream& eis )
{
    try
    {
        elem_t elem;
        eis >> elem;

        elpa_ostream( _out ) << elem << std::endl;
    }
    catch( const Error<Syntax,EOS>& )
    {
        throw MoreToRead();
    }
}

template<class Env>
void Shell<Env>::go()
{
    while( true )
    {
        std::string input;

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

                elpa_istream eis( iss, _alloc );
                read_command( eis );

                char c;
                while( iss >> c )
                    if( !isspace( c ) )
                        throw Error<Syntax>( "unexpected character after expression '"s + c + "'" );

                break;
            }
            catch( const MoreToRead& )
            {
            }
        }
    }
}

template class Shell< Env<Debug, SimpleScheme, SimpleAllocator> >;
