#ifndef ELPA_SHELL_H
#define ELPA_SHELL_H

#include "memory.h"
#include "stream.h"
#include "interpreter.h"
#include "container/elpa_map.h"
#include <iostream>
#include <iomanip>

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
    typedef typename elpa_ostream::Manip Manip;
    typedef typename elpa_ostream::BaseManip BaseManip;
    template<class K, class V>
    using elpa_map = typename Env::template elpa_map<K, V>;

    ElpaManip _format;
    BaseManip _num_format;
    bool _line_format;

    ShellManager _manager;
    elpa_map<std::string, elem_t> _defns;   //put in manager?

    auto process_command( const std::string& cmd, elpa_istream& eis, elpa_ostream& eos ) -> bool;
    auto process_input( std::istream& is, elpa_ostream& eos ) -> bool;
public:
    struct MoreToRead {};

    Shell()
        : _format( flat ), _num_format( std::dec ), _line_format( true ), _manager( 1024 ), _defns( _manager.interpreter().allocator() ) {}

    void interactive( std::istream& in, std::ostream& out );
	auto process( std::istream& in, std::ostream& out ) -> elem_t;
	auto process( const std::string& in, std::ostream& out ) -> elem_t;
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