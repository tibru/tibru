#ifndef KCON_SHELL_H
#define KCON_SHELL_H

#include "memory.h"
#include <iostream>

namespace kcon {

template<class Env>
class Shell
{
    typedef typename Env::Allocator Allocator;
    typedef typename Env::kistream kistream;
    typedef typename Env::kostream kostream;
    typedef typename Env::elem_t elem_t;

    std::istream& _in;
    std::ostream& _out;

    Allocator _alloc;
public:
    Shell( std::istream& in, std::ostream& out )
        : _in( in ), _out( out ), _alloc( 1024 ) {}

    void go();
};

}   //namespace

#endif
