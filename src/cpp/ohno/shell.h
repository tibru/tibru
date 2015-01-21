#ifndef OHNO_SHELL_H_INCLUDED
#define OHNO_SHELL_H_INCLUDED

#include "../kcon/shell.h"

namespace ohno
{

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
class OhNoShellManager : public kcon::KConShellManager<System, SchemeT, AllocatorT>
{
public:
};

}   //namespace

#endif // OHNO_SHELL_H_INCLUDED
