#include "interpreter.h"
#include <iomanip>

using namespace kcon;
using namespace elpa;

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
auto KConInterpreter<System,SchemeT,AllocatorT>::process_operator( char op, elpa_istream& eis, std::ostream& out ) -> bool
{
    //eis >> end;
    out << "OP " << op << std::endl;
    return true;
}

