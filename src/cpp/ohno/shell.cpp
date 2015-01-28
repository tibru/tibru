#include "shell.tpp"
#include "../elpa/shell.tpp"
#include "interpreter.h"
#include "../elpa/runtime.h"

using namespace ohno;
using namespace elpa;

template class OhNoShellManager<Debug, SimpleScheme, TestAllocator>;
template class OhNoShellManager<Debug, SimpleScheme, SimpleAllocator>;
template class OhNoShellManager<Safe, OptScheme, OptAllocator>;
template class OhNoShellManager<Fast, OptScheme, OptAllocator>;

template class Shell< Env<Debug, SimpleScheme, TestAllocator, OhNoInterpreter> >;
template class Shell< Env<Debug, SimpleScheme, SimpleAllocator, OhNoInterpreter> >;
template class Shell< Env<Safe, OptScheme, OptAllocator, OhNoInterpreter> >;
template class Shell< Env<Fast, OptScheme, OptAllocator, OhNoInterpreter> >;
