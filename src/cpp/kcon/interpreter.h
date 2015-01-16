#ifndef KCON_INTERPRETER_H
#define KCON_INTERPRETER_H

#include "../elpa/interpreter.h"

using namespace elpa;

namespace kcon {

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
class KConShellManager;

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
class KConInterpreter : public InterpreterBase<System, SchemeT, AllocatorT>
{
    typedef SchemeT<System> Scheme;
    typedef AllocatorT<System,SchemeT> Allocator;
    typedef typename Scheme::pcell_t pcell_t;
    typedef typename Scheme::byte_t byte_t;
    typedef typename Scheme::elem_t elem_t;

    template<class T>
    using auto_root = typename Allocator::template auto_root<T>;

    static auto _parse_count( pcell_t bytes ) -> size_t;
    static auto _parse_path_elem( pcell_t path, size_t& tcount, byte_t& hcount ) -> pcell_t;

    auto _constant( elem_t env, elem_t k ) -> elem_t;
    auto _select( elem_t env, pcell_t path ) -> elem_t;
    auto _ifcell( pcell_t choices, elem_t cond ) -> elem_t;
    auto _evaluate( elem_t env, pcell_t expr ) -> elem_t;
    auto _reduce( elem_t env, pcell_t expr ) -> elem_t;
public:
    KConInterpreter( size_t ncells )
        : InterpreterBase<System, SchemeT, AllocatorT>( ncells ) {}

    typedef KConShellManager<System, SchemeT, AllocatorT> ShellManager;

    auto constant( elem_t elem ) -> elem_t;
    auto select( elem_t elem ) -> elem_t;
    auto ifcell( elem_t elem ) -> elem_t;
    auto evaluate( elem_t elem ) -> elem_t;
    auto reduce( elem_t elem ) -> elem_t;
    auto execute( elem_t state ) -> elem_t;
};

}   //namespace

#endif // HEADER_KCON_INTERPRETER
