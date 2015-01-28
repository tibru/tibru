#include "shell.tpp"
#include "../elpa/shell.tpp"
#include "interpreter.h"
#include "../elpa/runtime.h"

using namespace kcon;
using namespace elpa;

template class KConShellManager<Debug, SimpleScheme, TestAllocator>;
template class KConShellManager<Debug, SimpleScheme, SimpleAllocator>;
template class KConShellManager<Safe, OptScheme, OptAllocator>;
template class KConShellManager<Fast, OptScheme, OptAllocator>;

template class Shell< Env<Debug, SimpleScheme, TestAllocator, KConInterpreter> >;
template class Shell< Env<Debug, SimpleScheme, SimpleAllocator, KConInterpreter> >;
template class Shell< Env<Safe, OptScheme, OptAllocator, KConInterpreter> >;
template class Shell< Env<Fast, OptScheme, OptAllocator, KConInterpreter> >;
