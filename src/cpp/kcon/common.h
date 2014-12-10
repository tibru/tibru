#ifndef KCON_COMMON_H
#define KCON_COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <exception>
#include <sstream>

template<class Tag>
class Error : public std::exception
{
    std::string _msg;
public:
    Error( const std::string& msg ) : _msg( msg ) {}
    Error( const std::ostringstream& oss ) : _msg( oss.str() ) {}
};

struct Runtime;
typedef Error<Runtime> RuntimeError;

inline std::string operator""_s( const char* s, size_t l )
{
	return s;
}

inline void assert( int test, const char* fmt, ... )
{
	if( test )
		return;

	va_list argptr;
	va_start( argptr, fmt );
	vprintf( fmt, argptr );
	va_end( argptr );

	exit(1);
}

inline void assert( int test, const std::string& msg )
{
	assert( test, msg.c_str() );
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