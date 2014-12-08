#ifndef KCON_PRINTER_H
#define KCON_PRINTER_H

#include <ostream>
#include "types.h"

class Printer
{
	std::ostream& _os;
	bool _flatten;

	void _print( const Node<pcell_t,pcell_t>* pcell );
	void _print( const Node<pcell_t,value_t>* pcell );
	void _print( const Node<value_t,pcell_t>* pcell );
	void _print( const Node<value_t,value_t>* pcell );
	void _print( pcell_t pcell );
public:
	Printer( std::ostream& os, bool flatten=true )
		: _os( os ), _flatten( flatten ) {}

	std::ostream& operator<<( pcell_t pcell );
};

#endif
