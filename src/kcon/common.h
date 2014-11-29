#ifndef KCON_COMMON_H
#define KCON_COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

void error( const char* fmt, ... )
{
	va_list argptr;
	va_start( argptr, fmt );
	vprintf( fmt, argptr );
	va_end( argptr );

	exit(-1);
}

void assert( int test, const char* fmt, ... )
{
	if( test )
		return;

	va_list argptr;
	va_start( argptr, fmt );
	vprintf( fmt, argptr );
	va_end( argptr );

	exit(1);
}

#endif