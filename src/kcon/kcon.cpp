#include "common.h"
#include "types.h"

struct Allocator
{
	template<class H, class T>
	const Node<H,T>* alloc( H head, T tail )
	{
		return new Node<H,T>{ head, tail };
	}
};


#include "Printer.cpp"
#include <iostream>
#include <sstream>

void test_printer()
{
	Allocator a;
	pnode_t p = a.alloc<value_t,pnode_t>( 0, a.alloc<pnode_t,value_t>( a.alloc<value_t,value_t>(3,3), 2 ) );
	
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