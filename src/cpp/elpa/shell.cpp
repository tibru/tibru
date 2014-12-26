#include "shell.h"
#include "runtime.h"
#include <sstream>
#include <iomanip>
#include <set>

using namespace elpa;

template<class Env>
auto Shell<Env>::process_command( const std::string& cmd, elpa_istream& eis ) -> bool
{
	if( cmd == "def" )
    {
    	std::string name;
    	elem_t elem;
    	eis >> name >> elem >> nomoreinput;
    	_names[name] = elem;
    	return true;
    }
    
    eis >> nomoreinput;

    if( cmd == "quit" || cmd == "exit" )
        return false;
    else if( cmd == "flat" )
        _format = flat;
    else if( cmd == "deep" )
        _format = deep;
    else if( cmd == "line" )
        _line_format = true;
    else if( cmd == "list" )
        _line_format = false;
    else
        throw Error<Command>( "Unknown command '"s + cmd + "'" );

    return true;
}

template<class Env>
auto Shell<Env>::process_input( const std::string& input ) -> bool
{
    std::istringstream iss( input );
    elpa_istream eis( iss, _manager.interpreter().allocator(), _names );
	elpa_ostream eos( _out );

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
        else
        {
            if( !_manager.is_valid_operator( c ) )
            {
                eis.putback( c );
                c = '\0';
            }

            try
            {
                elem_t elem;
                eis >> elem >> nomoreinput;

                if( c != '\0' )
                    elem = _manager.process_operator( c, elem );
                    
                _names["it"] = elem;

				eos << _format;
                if( !_line_format )
                    for( ; elem.is_pcell(); elem = elem.pcell()->tail() )
                        eos << elem.pcell()->head() << std::endl;

                eos << elem << std::endl;

                return true;
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

#include "../kcon/interpreter.h"
#include "../kcon/shell.h"
template class Shell< Env<Debug, SimpleScheme, TestAllocator, kcon::KConInterpreter> >;