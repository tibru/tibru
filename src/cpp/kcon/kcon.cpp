#include "common.h"
#include "types.h"

struct Allocator
{
};

void* operator new( size_t size, Allocator& alloc )
{
	return new char[size];
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

int main( int argc, const char* argv[] )
{
	test_printer();
	std::cout << "** All tests passed **\n";
}