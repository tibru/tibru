#ifndef ELPA_SHELL_H
#define ELPA_SHELL_H

#include "memory.h"
#include "stream.h"
#include "interpreter.h"
#include "container/elpa_map.h"
#include <iostream>

namespace elpa {

struct Command;

template<class Interpreter>
struct ShellManager;

template<class Env>
class Shell
{
    typedef typename Env::Interpreter::ShellManager ShellManager;
    typedef typename Env::elpa_istream elpa_istream;
    typedef typename Env::elpa_ostream elpa_ostream;
    typedef typename Env::elem_t elem_t;
    typedef typename elpa_ostream::ElpaManip ElpaManip;
    template<class K, class V>
    using elpa_map = typename Env::template elpa_map<K, V>;

    std::istream& _in;
    std::ostream& _out;

    ElpaManip _format;
    bool _line_format;

    ShellManager _manager;
    elpa_map<std::string, elem_t> _defns;   //put in manager?

    auto process_command( const std::string& cmd, elpa_istream& eis ) -> bool;
    auto process_input( const std::string& input ) -> bool;
public:
    struct MoreToRead {};

    Shell( std::istream& in, std::ostream& out )
        : _in( in ), _out( out ), _format( flat ), _line_format( true ), _manager( 1024 ), _defns( _manager.interpreter().allocator() ) {}

    void go();
};

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT, MetaInterpreter class InterpreterT>
class ShellManagerBase
{
protected:
    typedef InterpreterT<System,SchemeT,AllocatorT> Interpreter;

    Interpreter _interpreter;

    ShellManagerBase( size_t ncells )
        : _interpreter( ncells ) {}
public:
    Interpreter& interpreter() { return _interpreter; }
};

}   //namespace

#endif