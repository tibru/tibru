#include "memory.tpp"
#include "runtime.h"

using namespace elpa;

template class TestAllocator< Debug, SimpleScheme >;
template class SimpleAllocator< Debug, SimpleScheme >;

template class SimpleAllocator< Safe, OptScheme >;  //should read OptAllocator when a real class
template class SimpleAllocator< Fast, OptScheme >;  //should read OptAllocator when a real class
