#ifndef OHNO_SHELL_H_INCLUDED
#define OHNO_SHELL_H_INCLUDED

#include "../kcon/shell.h"

namespace ohno
{

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
class OhNoShellManager : public kcon::KConShellManager<System, SchemeT, AllocatorT>
{
    typedef kcon::KConShellManager<System, SchemeT, AllocatorT> Base;
    typedef SchemeT<System> Scheme;
    typedef AllocatorT<System, SchemeT> Allocator;
    typedef typename elpa_istream<System, SchemeT, AllocatorT>::Readers Readers;
    typedef typename elpa_istream<System, SchemeT, AllocatorT>::Macros Macros;
    typedef typename Scheme::elem_t elem_t;
    typedef typename Scheme::byte_t byte_t;

    template<class T>
    using auto_root = typename Allocator::template auto_root<T>;

    static auto _count_undef( elem_t e ) -> size_t;
public:
    OhNoShellManager( size_t ncells )
        : Base( ncells ) {}

    static auto readers() -> const Readers&;
    static auto macros() -> const Macros&;
    static void print_help( elpa_ostream<System,SchemeT>& eos );

    auto process_command( const std::string& cmd, elpa_istream<System, SchemeT, AllocatorT>& eis, elpa_ostream<System, SchemeT>& eos, bool noisy ) -> bool;
    void print_commands( elpa_ostream<System, SchemeT>& eos ) const;
};

}   //namespace

#endif // OHNO_SHELL_H_INCLUDED
