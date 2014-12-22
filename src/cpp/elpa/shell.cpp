#include "shell.h"
#include "runtime.h"
#include <sstream>

using namespace elpa;

template<class Env>
auto Shell<Env>::end( elpa_istream& eis ) -> elpa_istream&
{
    char c;
    while( eis >> c )
        if( !isspace( c ) )
            throw Error<Syntax>( "unexpected character after expression '"s + c + "'" );

    return eis;
}

template<class Env>
auto Shell<Env>::process_command( elpa_istream& eis ) -> bool
{
    try
    {
        elem_t elem;
        eis >> elem >> end;

        elpa_ostream( _out ) << elem << std::endl;
    }
    catch( const Error<Syntax,EOS>& )
    {
        throw MoreToRead();
    }

    return true;
}

template<class Env>
void Shell<Env>::go()
{
    bool keep_processing = true;
    while( keep_processing )
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
                keep_processing = process_command( eis );
                break;
            }
            catch( const MoreToRead& )
            {
            }
        }
    }
}

template class Shell< Env<Debug, SimpleScheme, SimpleAllocator> >;
