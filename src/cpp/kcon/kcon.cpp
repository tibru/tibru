#include "common.h"
#include "types.h"
#include <iostream>

struct Allocator
{
};

void* operator new( size_t size, Allocator& alloc )
{
	return new char[size];
}

#include <istream>

class Parser
{
	Allocator& _alloc;
	
	pnode_t _parse_elems( std::istream& is );
public:
	Parser( Allocator& alloc )
		: _alloc( alloc ) {}
		
	pnode_t parse( std::istream& is );
};

pnode_t Parser::_parse_elems( std::istream& is )
{
	char c;
	if( is >> c )
	{
		if( c == ']' )
		{
			return pnode_t::null();
		}
		else if( c == '[' )
		{
			pnode_t elems = _parse_elems( is );
			pnode_t tail = _parse_elems( is );
			return new (_alloc) Node<pnode_t,pnode_t>{ elems, tail };
		}
		else if( isdigit( c ) )
		{
			is.putback( c );
			value_t value;
			if( (is >> value) && (value < 256) )
			{
				pnode_t tail = _parse_elems( is );
				return new (_alloc) Node<value_t,pnode_t>{ value, tail };
			}
			
			error( "Malformed byte" );	
		}
		
		error( "Unexpected '%c'", c );
	}
	
	error( "Unexpected end of input" );
	return pnode_t::null();
}

pnode_t Parser::parse( std::istream& is )
{
	char c;
	if( is >> c )
	{
		assert( c == '[', "Expected '['" );
		return _parse_elems( is );
	}
	
	error( "Unexpected end of input" );
	
	return pnode_t::null();
}

#include "Printer.cpp"
#include <iostream>
#include <sstream>

void test_printer()
{
	Allocator a;
	pnode_t p = new (a) Node<value_t,pnode_t>{ 
					0,
					new (a) Node<pnode_t,value_t>{ 
						new (a) Node<value_t,value_t>{3,3}, 
						2 } };
	
	std::ostringstream os;
	Printer( os ) << p;
	Printer( os, false ) << p;
	assert( os.str() == "[0 [3 3] 2][0 [[3 3] 2]]", "Incorrect printing found '%s'", os.str().c_str()  );
}

void test_parser()
{
	Allocator a;
	std::istringstream iss("[0 1]");
	Printer( std::cout ) << Parser( a ).parse( iss );
}

int main( int argc, const char* argv[] )
{
	test_printer();
	test_parser();
	std::cout << "** All tests passed **\n";
}