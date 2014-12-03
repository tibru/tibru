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

template<bool>
struct ASSERT_FAILED;

template<>
struct ASSERT_FAILED< true > {};

template< size_t n >
struct assert_test {};

#define APPLY(fn,x,y) fn(x,y)
#define CONCAT(x,y) x##y
#define ASSERT( cond ) typedef assert_test< sizeof( ASSERT_FAILED< ( cond ) > ) > APPLY( CONCAT, assert_test_type, __COUNTER__ )


#endif