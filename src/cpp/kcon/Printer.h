#ifndef KCON_PRINTER_H
#define KCON_PRINTER_H

#include <ostream>
#include "types.h"

class Printer
{
	std::ostream& _os;
	bool _flatten;
	
	void _print( const Node<pnode_t,pnode_t>* pnode );
	void _print( const Node<pnode_t,value_t>* pnode );
	void _print( const Node<value_t,pnode_t>* pnode );
	void _print( const Node<value_t,value_t>* pnode );
	void _print( pnode_t pnode );
public:
	Printer( std::ostream& os, bool flatten=true )
		: _os( os ), _flatten( flatten ) {}

	std::ostream& operator<<( pnode_t pnode );
};

#endif