#ifndef ELPA_SHELL_H
#define ELPA_SHELL_H

#include "memory.h"
#include "stream.h"
#include <iostream>

namespace elpa {

template<class Env>
class Shell
{
    typedef typename Env::Allocator Allocator;
    typedef typename Env::elpa_istream elpa_istream;
    typedef typename Env::elpa_ostream elpa_ostream;
    typedef typename Env::elem_t elem_t;

    std::istream& _in;
    std::ostream& _out;

    Allocator _alloc;

    auto process_command( const std::string& command ) -> bool;
    static auto end( elpa_istream& ) -> elpa_istream&;
public:
    struct MoreToRead {};

    Shell( std::istream& in, std::ostream& out )
        : _in( in ), _out( out ), _alloc( 1024 ) {}

    void go();
};

}   //namespace

#endif
