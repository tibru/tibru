#define NO_REPLACE_NEW

#include "common.h"

void* operator_new( size_t size ) throw(std::bad_alloc)
{
    return operator new( size );
}

