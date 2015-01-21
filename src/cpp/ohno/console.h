#ifndef OHNO_CONSOLE_H_INCLUDED
#define OHNO_CONSOLE_H_INCLUDED

#include "../elpa/console.h"
#include "../kcon/tests.h"
#include "interpreter.h"
#include "tests.h"

namespace ohno {

struct OhNoConsole : elpa::Console<OhNoInterpreter>
{
    virtual void run_tests() const
    {
        kcon::run_tests();
        ohno::run_tests();
    }
};

}   //namespace

#endif // OHNO_CONSOLE_H_INCLUDED
