#include "shell.h"
#include "runtime.h"
#include <sstream>
#include <iomanip>

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
auto Shell<Env>::process_command( const std::string& input ) -> bool
{
    std::istringstream iss( input );
    elpa_istream eis( iss, _alloc );

    char c;
    eis >> c;

    std::string command = "";
    if( c != ':' )
        eis.putback( c );
    else
        if( !(eis >> std::noskipws >> command >> std::skipws) )
            throw Error<Syntax>( "Expected command after ':'" );

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

                keep_processing = process_command( input );
                break;
            }
            catch( const MoreToRead& )
            {
            }
        }
    }
}

template class Shell< Env<Debug, SimpleScheme, SimpleAllocator> >;
