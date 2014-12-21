#ifndef KCON_COMMON_H
#define KCON_COMMON_H

#include <exception>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>

namespace kcon {

using namespace std::string_literals;

struct AnyType;

template<class Type, class SubType=AnyType>
class Error;

template<>
template<class Type>
class Error<Type,AnyType> : public std::exception
{
    std::string _message;
public:
    Error( const std::string& message )
    	: _message( message ) {}

	const char* what() const throw() { return ('\n' + _message).c_str(); }
	const std::string& message() const { return _message; }
};

template<class Type, class SubType>
struct Error : Error<Type>
{
	Error( const std::string& message )
    	: Error<Type>( message ) {}
};

struct Assertion;
struct Test;

inline void fail( const std::string& msg )
{
	throw Error<Test>( msg );
}

inline void pass()
{
	std::cout << '.' << std::flush;
}

inline void test( bool cond, const std::string& msg )
{
	if( !cond )
		fail( msg );

    pass();
}

#define TEST std::cout << "\n" << __FUNCTION__ << ": " << std::flush;

template<bool>
struct ASSERT_FAILED;

template<>
struct ASSERT_FAILED< true > {};

template< size_t n >
struct assert_test {};

#define APPLY(fn,x,y) fn(x,y)
#define CONCAT(x,y) x##y
#define ASSERT( cond ) typedef assert_test< sizeof( ASSERT_FAILED< ( cond ) > ) > APPLY( CONCAT, assert_test_type, __COUNTER__ )

inline auto extract_type_name( const char* name ) -> auto
{
    std::string s = name;
    const size_t n = std::min( s.find( ';' ), s.find( ']' ) );
    return s.substr( 33, n-33 );
}

template<typename T>
inline auto type_name() -> auto
{
    static std::string s = extract_type_name( __PRETTY_FUNCTION__ );
    return s;
}

#define TYPENAME( ... ) type_name<__VA_ARGS__>()

template<class T>
auto operator+( const std::string& s, const T& t ) -> std::string
{
    std::ostringstream oss;
    oss << s << t;
    return oss.str();
}

#define here(x) std::cout << "here" << x << std::endl;

const uintptr_t HI_ADDR_BIT = ~(uintptr_t(-1) >> 1);
const uintptr_t THREE_GB = 0xC0000000;
const uintptr_t MAX_POINTER = sizeof(void*) == 8 ? HI_ADDR_BIT : THREE_GB;

}	//namespace




#endif
