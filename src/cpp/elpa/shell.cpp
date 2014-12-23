#include "shell.h"
#include "runtime.h"
#include <sstream>
#include <iomanip>
#include <set>

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
auto Shell<Env>::process_operator( char op, elpa_istream& eis ) -> bool
{
    eis >> end;
    _out << "OP " << op << std::endl;
    return true;
}

template<class Env>
auto Shell<Env>::process_command( const std::string& cmd, elpa_istream& eis ) -> bool
{
    eis >> end;

    if( cmd == "quit" || cmd == "exit" )
        return false;

    throw Error<Command>( "Unknown command '"s + cmd + "'" );
}

template<class Env>
auto Shell<Env>::process_input( const std::string& input ) -> bool
{
    std::istringstream iss( input );
    elpa_istream eis( iss, _alloc );

    const std::set<char> OPS = {'*','/'};

    char c;
    if( eis >> c )
    {
        if( c == ':' )
        {
            std::string cmd;
            if( !(eis >> std::noskipws >> cmd >> std::skipws) )
                throw Error<Syntax>( "Expected command after ':'" );

            return process_command( cmd, eis );
        }
        else if( OPS.find( c ) != OPS.end() )
        {
            return process_operator( c, eis );
        }
        else
        {
            eis.putback( c );

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
        }
    }

    return true;
}

template<class Env>
void Shell<Env>::go()
{
    bool keep_processing = true;
    while( keep_processing )
    {
        try
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

                    keep_processing = process_input( input );
                    break;
                }
                catch( const MoreToRead& )
                {
                }
            }
        }
        catch( const Error<Syntax>& e )
        {
            _out << "Syntax: " << e.message() << std::endl;
        }
        catch( const Error<Command>& e )
        {
            _out << "Command: " << e.message() << std::endl;
        }
    }
}

template class Shell< Env<Debug, SimpleScheme, SimpleAllocator> >;
