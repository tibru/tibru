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

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT, MetaInterpreter class InterpreterT>
class ShellManagerBase
{
    ShellManagerBase( const ShellManagerBase& );
    ShellManagerBase& operator=( const ShellManagerBase& );
protected:
    typedef InterpreterT<System,SchemeT,AllocatorT> Interpreter;
    typedef typename elpa_istream<System, SchemeT, AllocatorT>::Readers Readers;
    typedef typename elpa_istream<System, SchemeT, AllocatorT>::Macros Macros;
    typedef std::vector<std::string> Operators;
    typedef SchemeT<System> Scheme;
    typedef typename Scheme::elem_t elem_t;

	template<class K, class V>
    using elpa_map = container::elpa_map<System, SchemeT, AllocatorT, K, V>;

    typedef elpa_map<std::string,elem_t> Constants;

    Interpreter _interpreter;
    Constants _constants;
    Readers _readers;
    Macros _macros;
    Operators _operators;

    ShellManagerBase( size_t ncells )
        : _interpreter( ncells ), _constants( _interpreter.allocator() ) {}

    void _def_constant( const std::string& name, elem_t elem )
    {
        _constants[name] = elem;
    }

    auto _get_constant( const std::string& name ) const -> elem_t
    {
        System::assert( _constants.find( name ) != _constants.end(), "Failed to find constant '"s + name + "'" );
        return _constants.at( name );
    }

    void _def_reader( char c, const typename Readers::mapped_type& reader )
    {
        _readers[c] = reader;
    }

    void _def_macro( char c, const typename Macros::mapped_type& macro )
    {
        _macros[c] = macro;
    }

    void _def_operator( const std::string& op )
    {
        _operators.push_back( op );
    }
public:
    Interpreter& interpreter() { return _interpreter; }

    auto constants() const -> const Constants& { return _constants; }
    auto readers() -> const Readers& { return _readers; }
    auto macros() -> const Macros& { return _macros; }
    auto operators() -> const Operators& { return _operators; }

    virtual auto process_command( const std::string& cmd, elpa_istream<System, SchemeT, AllocatorT>& eis, elpa_ostream<System, SchemeT>& eos, bool noisy ) -> bool { return false; }
    virtual void print_commands( elpa_ostream<System, SchemeT>& eos ) const {}
};

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
class NullShellManager : public ShellManagerBase<System, SchemeT, AllocatorT, NullInterpreter>
{
public:
    typedef SchemeT<System> Scheme;
    typedef typename elpa_istream<System, SchemeT, AllocatorT>::Readers Readers;
    typedef typename elpa_istream<System, SchemeT, AllocatorT>::Macros Macros;
    typedef typename ShellManagerBase<System, SchemeT, AllocatorT, NullInterpreter>::Operators Operators;
    typedef typename Scheme::elem_t elem_t;

    NullShellManager( size_t ncells )
        : ShellManagerBase<System, SchemeT, AllocatorT, NullInterpreter>( ncells ) {}

    virtual void print_help( elpa_ostream<System,SchemeT>& eos ) {}

    auto process_operator( char op, elem_t elem, size_t niter, bool& more ) -> elem_t { more = false; return elem; }
};

template<class Env>
struct Shell
{
	typedef typename Env::System System;
	typedef typename Env::Scheme Scheme;
    typedef typename Env::Interpreter::ShellManager ShellManager;
    typedef typename Env::elpa_istream elpa_istream;
    typedef typename Env::elpa_ostream elpa_ostream;
    typedef typename Env::elem_t elem_t;
    typedef typename Env::byte_t byte_t;
    typedef typename elpa_ostream::ElpaManip ElpaManip;
    typedef typename elpa_ostream::Manip Manip;
    typedef typename elpa_ostream::BaseManip BaseManip;
    typedef typename ShellManager::Operators Operators;

    template<class K, class V>
    using elpa_map = typename Env::template elpa_map<K, V>;
private:
    ElpaManip _format;
    BaseManip _num_format;
    bool _line_format;
    bool _use_names;

    ShellManager _manager;
    elpa_map<std::string, elem_t> _defns_no_it;
    elpa_map<std::string, elem_t> _defns_with_it;
    elpa_map<std::string, elem_t> _defns_none;
    std::map<uint8_t, uint8_t> _defns_byte_counts;
    std::set<std::string> _processing;

    void _print( elpa_ostream& eos, elem_t elem );
    auto _process_command( const std::string& cmd, elpa_istream& eis, elpa_ostream& eos, bool noisy ) -> bool;
    auto _process_input( std::istream& is, elpa_ostream& eos, bool noisy=true ) -> bool;
public:
    struct MoreToRead {};

    Shell( size_t ncells )
        : _format( flat ), _num_format( std::dec ), _line_format( true ), _use_names( false ), _manager( ncells ),
          _defns_no_it( _manager.interpreter().allocator() ), _defns_with_it( _manager.interpreter().allocator() ), _defns_none( _manager.interpreter().allocator() )
    {
        for( auto c : _manager.constants() )
        {
            _defns_no_it[c.first] = c.second;
            _defns_with_it[c.first] = c.second;

            if( c.second.is_byte() )
            {
                byte_t b = c.second.byte();
                uint8_t v = Scheme::byte_value( b );
                uint8_t t = Scheme::byte_tag( b );
                _defns_byte_counts[v] = std::max<uint8_t>( _defns_byte_counts[v], t );
            }
        }
    }

    auto manager() -> ShellManager& { return _manager; }
    auto manager() const -> const ShellManager& { return _manager; }

    void interactive( std::istream& in, std::ostream& out );
    auto process( std::istream& in, elpa_ostream& eos ) -> elem_t;
	auto process( std::istream& in ) -> elem_t;
	void process( const std::string& filename, elpa_ostream& eos );
	void process( const std::string& filename );
	auto parse( const std::string& in ) -> elem_t;

	const elpa_map<std::string, elem_t>& names() const
	{
	    return _use_names ? _defns_no_it : _defns_none;
    }
};

}   //namespace

#endif
