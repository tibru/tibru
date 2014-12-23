#include "shell.h"

using namespace kcon;

template<>
template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
auto KConShellManager<System, SchemeT, AllocatorT>::process_operator( char op, elpa_istream& eis, std::ostream& out ) -> bool
{
    //eis >> end;
    out << "OP " << op << std::endl;
    return true;
}
