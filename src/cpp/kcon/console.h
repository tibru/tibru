#ifndef KCON_CONSOLE_H_INCLUDED
#define KCON_CONSOLE_H_INCLUDED

#include "../elpa/console.h"
#include "interpreter.h"
#include "tests.h"

namespace kcon {

struct KConConsole : elpa::Console<KConInterpreter>
{
    virtual void run_tests() const
    {
        kcon::run_tests();
    }
};

}   //namespace

#endif // KCON_CONSOLE_H_INCLUDED
