#ifndef ELPA_CONSOLE_H_INCLUDED
#define ELPA_CONSOLE_H_INCLUDED

#include "common.h"
#include "interpreter.h"

namespace elpa {

template<MetaInterpreter class Interpreter>
struct Console
{
    virtual auto help( int ret_code ) -> int;
    virtual void run_tests() const = 0;

    template<class Env>
    auto run( size_t ncells, const std::vector< std::string >& filenames, bool noisy, bool repl ) -> int;

    auto go( int argc, const char* argv[] ) -> int;

};

}   //namespace

#endif // ELPA_CONSOLE_H_INCLUDED
