#include "stream.tpp"
#include "runtime.h"

using namespace elpa;

template class elpa_ostream< Debug, SimpleScheme >;
template class elpa_istream< Debug, SimpleScheme, SimpleAllocator >;
template class elpa_istream< Debug, SimpleScheme, TestAllocator >;

template class elpa_ostream< Safe, OptScheme >;
template class elpa_istream< Safe, OptScheme, OptAllocator >;

template class elpa_ostream< Fast, OptScheme >;
template class elpa_istream< Fast, OptScheme, OptAllocator >;
