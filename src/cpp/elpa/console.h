#ifndef CONSOLE_H_INCLUDED
#define CONSOLE_H_INCLUDED

#include "common.h"

namespace elpa {

struct Console
{
    static auto help( int ret_code ) -> int;

    template<class Env>
    static auto run( size_t ncells, const std::vector< std::string >& filenames, bool noisy, bool repl ) -> int;

    static auto go( int argc, const char* argv[] ) -> int;
};

}   //namespace

#endif // CONSOLE_H_INCLUDED
