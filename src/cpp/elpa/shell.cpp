#include "shell.h"
#include "runtime.h"
#include <sstream>
#include <iomanip>
#include <set>
#include <fstream>
#include <algorithm>

using namespace elpa;

template<class Env>
auto Shell<Env>::_process_command( const std::string& cmd, elpa_istream& eis, elpa_ostream& eos, bool noisy ) -> bool
{
	if( cmd == "def" )
    {
    	std::string name;
    	elem_t elem;
    	eis >> name >> elem >> endofline;
    	_defns[name] = elem;
    	return true;
    }
    else if( cmd == "include" )
    {
    	std::string filename;
    	eis >> filename >> endofline;
    	include( filename );
    	return true;
    }

    eis >> endofline;

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
    	if( noisy )
    		for( auto defn : _defns )
    			eos << defn.first << std::endl;
    }
    else if( cmd == "sys" )
    {
    	if( noisy )
    	{
    		eos << "System: " << Env::System::name() << std::endl;
    		eos << "Scheme: " << Env::Scheme::name() << std::endl;
    		eos << "Allocator: " << Env::Allocator::name() << std::endl;
    		uint64_t nalloc = _manager.interpreter().allocator().num_allocated();
    		uint64_t ntotal = _manager.interpreter().allocator().num_total();
    		uint64_t nmax = _manager.interpreter().allocator().num_max();
    		eos << "Mem alloc: " << nalloc << " cells";
    		if( ntotal > 0 )
    			eos << " (" << (nalloc * 100) / ntotal << "%)";
    		eos << std::endl;
    		eos << "Mem total: " << nalloc << " cells" << std::endl;
    		eos << "Mem max: " << nmax << " cells" << std::endl;
    		eos << "GC count: " << _manager.interpreter().allocator().gc_count() << std::endl;
    	}
    }
    else if( cmd == "gc" )
    	_manager.interpreter().allocator().gc();
    else if( cmd == "help" )
    {
        if( noisy )
        {
            eos << "Evaluate an expression of the form <name>|byte|[<expr> <expr>+]|<reader><expr>|<expr><macro> and define 'it' as its value\n";
            eos << "Or process an operator on an expression of the form <op><expr> and define 'it' as its value\n";
            eos << "Or run a command\n";

            eos << "\nCommands:\n";
            eos << ":def <name> <expr> - Define a named expression\n";
            eos << ":include <filename> - Silently include all statements in the specified file\n";

            eos << ":dec  - Show bytes in decimal notation (default)\n";
            eos << ":hex  - Show bytes in hex notation\n";
            eos << ":flat - Show cells as flattened right associative lists (default)\n";
            eos << ":deep - Show cells as pairs\n";
            eos << ":line - Show expressions on a single line (default)\n";
            eos << ":list - Show expressions over multiple lines in list format\n";
            eos << ":defs - Show all defined names\n";
            eos << ":sys  - Show information about the system\n";
            eos << ":gc   - Run the garbage collector\n";
            eos << ":help - Show this help\n";
            eos << ":exit - End the shell session\n";
            eos << ":quit - End the shell session\n";

            eos << "\nOperators:\n";
    		for( auto op : _manager.operators() )
    			eos << op.first << " - " << op.second << std::endl;

            _manager.print_help( eos );
        }
    }
    else
        throw Error<Command>( "Unknown command '"s + cmd + "'" );

    return true;
}

template<class Env>
auto Shell<Env>::_process_input( std::istream& is, elpa_ostream& eos, bool noisy ) -> bool
{
    elpa_istream eis( is, _manager.interpreter().allocator(), _defns, _manager.readers(), _manager.macros() );

    char c;
    if( eis >> c )
    {
        if( c == ':' )
        {
            std::string cmd;
            if( !(eis >> std::noskipws >> cmd >> std::skipws) )
                throw Error<Syntax>( "Expected command after ':'" );

            return _process_command( cmd, eis, eos, noisy );
        }
        else
        {
            const Operators& ops = _manager.operators();
            if( std::find_if( ops.begin(), ops.end(), [c]( const Operators::value_type& v ){ return v.first == c; } ) == ops.end() )
            {
                eis.putback( c );
                c = '\0';
            }

            try
            {
                elem_t elem;
                eis >> elem >> endofline;

                if( c != '\0' )
                    elem = _manager.process_operator( c, elem );

                _defns["it"] = elem;

                if( noisy )
            	{
					eos << _format << _num_format;
                	if( !_line_format )
                	    for( ; elem.is_pcell(); elem = elem.pcell()->tail() )
                  	      eos << elem.pcell()->head() << std::endl;

                	eos << elem << std::endl;
            	}

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

					std::istringstream iss( input );
                    keep_processing = _process_input( iss, eos );
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
            out << "Run :help for more details" << std::endl;
        }
        catch( const Error<IllegalOp>& e )
        {
            out << "IllegalOp: " << e.message() << std::endl;
            out << "Run :help for more details" << std::endl;
        }
        catch( const Error<Command>& e )
        {
            out << "Command: " << e.message() << std::endl;
            out << "Run :help for more details" << std::endl;
        }
    }
}


template<class Env>
auto Shell<Env>::process( std::istream& in ) -> elem_t
{
	std::ostringstream oss;
	elpa_ostream eos( oss );
	while( in )
		_process_input( in, eos, false );
	System::assert( oss.str() == "", "Processing produced output" );
	return _defns.find("it") != _defns.end() ? _defns["it"] : elem_t();
}

template<class Env>
auto Shell<Env>::process( const std::string& in ) -> elem_t
{
	std::istringstream iss( in );
	return process( iss );
}

template<class Env>
void Shell<Env>::include( const std::string& filename )
{
	std::ifstream ifs( filename );
	if( !ifs )
		throw Error<Runtime>( "File not found '"s + filename + "'" );

	if( !_processing.insert( filename ).second )
		throw Error<Runtime>( "File included recursively '"s + filename + "'" );

	process( ifs );

	System::assert( _processing.erase( filename ) == 1, "Failed to register processed file" );
}

template class Shell< Env<Debug, SimpleScheme, TestAllocator, NullInterpreter> >;
template class Shell< Env<Debug, SimpleScheme, SimpleAllocator, NullInterpreter> >;

#include "../kcon/interpreter.h"
#include "../kcon/shell.h"
template class Shell< Env<Debug, SimpleScheme, TestAllocator, kcon::KConInterpreter> >;
template class Shell< Env<Debug, SimpleScheme, SimpleAllocator, kcon::KConInterpreter> >;
template class Shell< Env<Safe, OptScheme, OptAllocator, kcon::KConInterpreter> >;
template class Shell< Env<Fast, OptScheme, OptAllocator, kcon::KConInterpreter> >;
