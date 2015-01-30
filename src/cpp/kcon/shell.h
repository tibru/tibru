#ifndef KCON_SHELL_H
#define KCON_SHELL_H

#include "../elpa/shell.h"
#include "interpreter.h"

namespace kcon
{

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
class KConShellManager : public elpa::ShellManagerBase<System, SchemeT, AllocatorT, KConInterpreter>
{
    size_t _trace_limit;
public:
    typedef SchemeT<System> Scheme;
    typedef AllocatorT<System, SchemeT> Allocator;
    typedef typename elpa_istream<System, SchemeT, AllocatorT>::Readers Readers;
    typedef typename elpa_istream<System, SchemeT, AllocatorT>::Macros Macros;
    typedef typename ShellManagerBase<System, SchemeT, AllocatorT, KConInterpreter>::Operators Operators;
    typedef typename Scheme::elem_t elem_t;
    typedef typename Scheme::byte_t byte_t;

    template<class T>
    using auto_root = typename Allocator::template auto_root<T>;

    KConShellManager( size_t ncells );

    auto process_operator( char op, elem_t elem, size_t niter, bool& more ) -> elem_t;
    auto process_command( const std::string& cmd, elpa_istream<System, SchemeT, AllocatorT>& eis, elpa_ostream<System, SchemeT>& eos, bool noisy ) -> bool;
    void print_commands( elpa_ostream<System, SchemeT>& eos ) const;
     void print_help( elpa_ostream<System,SchemeT>& eos );
};

}   //namespace

#endif
