#ifndef SNABL_LIB_ABC_HPP
#define SNABL_LIB_ABC_HPP

#include "snabl/lib.hpp"

namespace snabl::libs {
  struct Abc: Lib {
    Abc(Env &env);
    void init();
  };
}

#endif
