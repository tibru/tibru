#include "common.h"
#include "types.h"

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
public:
	Parser( Allocator& alloc )
		: _alloc( alloc ) {}
		
	pnode_t parse( std::istream& is );
};

pnode_t Parser::parse( std::istream& is )
{
	return new (_alloc) Node<value_t,value_t>{ 0, 1 };
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
	assert( os.str() == "[0 [3 3] 2][0 [[3 3] 2]]", "Incorrect printing" );
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