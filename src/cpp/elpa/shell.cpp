#include "shell.tpp"
#include "runtime.h"

using namespace elpa;

template class Shell< Env<Debug, SimpleScheme, TestAllocator, NullInterpreter> >;
template class Shell< Env<Debug, SimpleScheme, SimpleAllocator, NullInterpreter> >;
