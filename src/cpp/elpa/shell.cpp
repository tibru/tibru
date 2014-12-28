#include "shell.h"
#include "runtime.h"
#include <sstream>
#include <iomanip>
#include <set>

using namespace elpa;

template<class Env>
auto Shell<Env>::process_command( const std::string& cmd, elpa_istream& eis, elpa_ostream& eos ) -> bool
{
	if( cmd == "def" )
    {
    	std::string name;
    	elem_t elem;
    	eis >> name >> elem >> nomoreinput;
    	_defns[name] = elem;
    	return true;
    }
    
    eis >> nomoreinput;

    if( cmd == "quit" || cmd == "exit" )
        return false;
    else if( cmd == "dec" )
        _num_format = std::dec;
    else if( cmd == "hex" )
        _num_format = std::hex;
    else if( cmd == "flat" )
        _format = flat;
    else if( cmd == "deep" )
        _format = deep;
    else if( cmd == "line" )
        _line_format = true;
    else if( cmd == "list" )
        _line_format = false;
    else if( cmd == "defs" )
    {
    	for( auto defn : _defns )
    		eos << defn.first << std::endl;
    }
    else if( cmd == "sys" )
    {
    	eos << "System: " << Env::System::name() << std::endl;
    	eos << "Scheme: " << Env::Scheme::name() << std::endl;
    	eos << "Allocator: " << Env::Allocator::name() << std::endl;
    	uint64_t nalloc = _manager.interpreter().allocator().num_allocated();
    	uint64_t ntotal = _manager.interpreter().allocator().num_total();
    	eos << "Mem alloc: " << nalloc << " cells";
    	if( ntotal > 0 )
    		eos << " (" << (nalloc * 100) / ntotal << "%)";
    	eos << std::endl;
    	eos << "Mem total: " << nalloc << " cells" << std::endl;
    	eos << "GC count: " << _manager.interpreter().allocator().gc_count() << std::endl;
    }
    else if( cmd == "gc" )
    	_manager.interpreter().allocator().gc();
    else
        throw Error<Command>( "Unknown command '"s + cmd + "'" );

    return true;
}

template<class Env>
auto Shell<Env>::process_input( const std::string& input, elpa_ostream& eos ) -> bool
{
    std::istringstream iss( input );
    elpa_istream eis( iss, _manager.interpreter().allocator(), _defns );

    char c;
    if( eis >> c )
    {
        if( c == ':' )
        {
            std::string cmd;
            if( !(eis >> std::noskipws >> cmd >> std::skipws) )
                throw Error<Syntax>( "Expected command after ':'" );

            return process_command( cmd, eis, eos );
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
                    
                _defns["it"] = elem;

				eos << _format << _num_format;
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
void Shell<Env>::interactive( std::istream& in, std::ostream& out )
{
	elpa_ostream eos( out );
	
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
                    out << prompt;
                    std::string line;
                    std::getline( in, line );
                    input += (line + "\n"s);
                    prompt = "... ";

                    keep_processing = process_input( input, eos );
                    break;
                }
                catch( const MoreToRead& )
                {
                }
            }
        }
        catch( const Error<Syntax>& e )
        {
            out << "Syntax: " << e.message() << std::endl;
        }
        catch( const Error<Command>& e )
        {
            out << "Command: " << e.message() << std::endl;
        }
    }
}


template<class Env>
void Shell<Env>::process( std::istream& in, std::ostream& out )
{
}

#include "../kcon/interpreter.h"
#include "../kcon/shell.h"
template class Shell< Env<Debug, SimpleScheme, TestAllocator, kcon::KConInterpreter> >;