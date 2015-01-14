#ifndef ELPA_SHELL_H
#define ELPA_SHELL_H

#include "memory.h"
#include "stream.h"
#include "interpreter.h"
#include "container/elpa_map.h"
#include <iostream>
#include <iomanip>
#include <set>

namespace elpa {

struct Command;

template<class Interpreter>
struct ShellManager;

template<class Env>
struct Shell
{
	typedef typename Env::System System;
    typedef typename Env::Interpreter::ShellManager ShellManager;
    typedef typename Env::elpa_istream elpa_istream;
    typedef typename Env::elpa_ostream elpa_ostream;
    typedef typename Env::elem_t elem_t;
    typedef typename elpa_ostream::ElpaManip ElpaManip;
    typedef typename elpa_ostream::Manip Manip;
    typedef typename elpa_ostream::BaseManip BaseManip;
    typedef std::vector< std::pair<char,std::string> > Operators;
private:
    ElpaManip _format;
    BaseManip _num_format;
    bool _line_format;

    ShellManager _manager;
    std::set<std::string> _processing;

    auto _process_command( const std::string& cmd, elpa_istream& eis, elpa_ostream& eos, bool noisy ) -> bool;
    auto _process_input( std::istream& is, elpa_ostream& eos, bool noisy=true ) -> bool;
public:
    struct MoreToRead {};

    Shell( size_t ncells )
        : _format( flat ), _num_format( std::dec ), _line_format( true ), _manager( ncells ) {}

    void interactive( std::istream& in, std::ostream& out );
	auto process( std::istream& in ) -> elem_t;
	auto process( const std::string& in ) -> elem_t;
	void include( const std::string& filename );
};

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT, MetaInterpreter class InterpreterT>
class ShellManagerBase
{
protected:
    typedef SchemeT<System> Scheme;
    typedef InterpreterT<System,SchemeT,AllocatorT> Interpreter;
    typedef typename Scheme::elem_t elem_t;

    template<class K, class V>
    using elpa_map = container::elpa_map<System, SchemeT, AllocatorT, K, V>;

    Interpreter _interpreter;
    elpa_map<std::string, elem_t> _defns;

    ShellManagerBase( size_t ncells )
        : _interpreter( ncells ), _defns( _interpreter.allocator() )
    {
        _defns["it"] = elem_t();
    }
public:
    Interpreter& interpreter() { return _interpreter; }

    auto def( const std::string& name ) const -> elem_t { return _defns.at(name); }
    void def( const std::string& name, elem_t elem ) { _defns[name] = elem; }
    auto defs() const -> const auto& { return _defns; }
};

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
class NullShellManager : public ShellManagerBase<System, SchemeT, AllocatorT, NullInterpreter>
{
public:
    typedef SchemeT<System> Scheme;
    typedef typename elpa_istream<System, SchemeT, AllocatorT>::Readers Readers;
    typedef typename elpa_istream<System, SchemeT, AllocatorT>::Macros Macros;
    typedef std::vector< std::pair<char,std::string> > Operators;
    typedef typename Scheme::elem_t elem_t;

    NullShellManager( size_t ncells )
        : ShellManagerBase<System, SchemeT, AllocatorT, NullInterpreter>( ncells ) {}

    static auto readers() -> const Readers& { static Readers readers; return readers; }
    static auto macros() -> const Macros& { static Macros macros; return macros; }
    static auto operators() -> const Operators& { static const Operators ops; return ops; }
    static void print_help( elpa_ostream<System,SchemeT>& eos ) {}

    auto process_operator( char op, elem_t elem ) -> elem_t { return elem; }
};

}   //namespace

#endif
