#ifndef KCON_SHELL_H
#define KCON_SHELL_H

#include "../elpa/shell.h"
#include "interpreter.h"

namespace kcon
{

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
class KConShellManager : public elpa::ShellManagerBase<System, SchemeT, AllocatorT, KConInterpreter>
{
public:
    typedef SchemeT<System> Scheme;
    typedef typename elpa_istream<System, SchemeT, AllocatorT>::Readers Readers;
    typedef typename elpa_istream<System, SchemeT, AllocatorT>::Macros Macros;
    typedef typename Scheme::elem_t elem_t;

    KConShellManager( size_t ncells )
        : elpa::ShellManagerBase<System, SchemeT, AllocatorT, KConInterpreter>( ncells ) {}

    static auto readers() -> const Readers&;
    static auto macros() -> const Macros&;
    static auto operators() -> const std::vector<char>&;

    auto process_operator( char op, elem_t elem ) -> elem_t;
};

}   //namespace

#endif
