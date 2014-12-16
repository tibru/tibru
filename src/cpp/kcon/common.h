#ifndef KCON_COMMON_H
#define KCON_COMMON_H

#include <exception>
#include <string>
#include <iostream>

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

inline void assert( bool cond, const std::string& msg )
{
	if( !cond )
		throw Error<Assertion>( msg );
}

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

inline std::string extract_type_name( const char* name )
{
    std::string s = name;
    const size_t n = s.find( ';' );
    return s.substr( 40, n-40 );
}

template<typename T>
inline std::string type_name()
{
    static std::string s = extract_type_name( __PRETTY_FUNCTION__ );
    return s;
}

#define TYPENAME( ... ) type_name<__VA_ARGS__>()

}	//namespace

#endif
