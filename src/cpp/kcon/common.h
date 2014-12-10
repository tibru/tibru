#ifndef KCON_COMMON_H
#define KCON_COMMON_H

#include <exception>
#include <string>

using namespace std::string_literals;

template<class Tag>
class Error : public std::exception
{
    std::string _message;
public:
    Error( const std::string& message ) : _message( message ) {}

	const char* what() const throw() { return ('\n' + _message).c_str(); }
	const std::string& message() const { return _message; }
};

struct Runtime;
struct Assertion;
struct Test;

inline void assert( bool cond, const std::string& msg )
{
	if( !cond )
		throw Error<Assertion>( msg );
}

inline void test( bool cond, const std::string& msg )
{
	if( !cond )
		throw Error<Test>( msg );
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
