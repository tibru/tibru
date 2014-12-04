#ifndef HEADER_KCON_ALLOCATOR
#define HEADER_KCON_ALLOCATOR

#include "types.h"
#include <cstdint>

struct Allocator
{
};

inline void* operator new( size_t size, Allocator& alloc )
{
	return new char[size];
}

#endif