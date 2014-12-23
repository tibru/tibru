#ifndef ELPA_SHELL_H
#define ELPA_SHELL_H

#include "memory.h"
#include "stream.h"
#include <iostream>

namespace elpa {

struct Command;

template<class Env>
class Shell
{
    typedef typename Env::Interpreter Interpreter;
    typedef typename Env::elpa_istream elpa_istream;
    typedef typename Env::elpa_ostream elpa_ostream;
    typedef typename Env::elem_t elem_t;

    std::istream& _in;
    std::ostream& _out;

    Interpreter _interpreter;

    auto process_operator( char op, elpa_istream& eis ) -> bool;
    auto process_command( const std::string& cmd, elpa_istream& eis ) -> bool;
    auto process_input( const std::string& input ) -> bool;
    static auto end( elpa_istream& ) -> elpa_istream&;
public:
    struct MoreToRead {};

    Shell( std::istream& in, std::ostream& out )
        : _in( in ), _out( out ), _interpreter( 1024 ) {}

    void go();
};

}   //namespace

#endif
