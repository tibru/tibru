#ifndef ELPA_SHELL_H
#define ELPA_SHELL_H

#include "memory.h"
#include "stream.h"
#include "interpreter.h"
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

    std::istream& _in;
    std::ostream& _out;

    ShellManager _manager;

    auto process_command( const std::string& cmd, elpa_istream& eis ) -> bool;
    auto process_input( const std::string& input ) -> bool;
public:
    struct MoreToRead {};

    Shell( std::istream& in, std::ostream& out )
        : _in( in ), _out( out ), _manager( 1024 ) {}

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
