#ifndef SNABL_LIB_HOME_HPP
#define SNABL_LIB_HOME_HPP

#include "snabl/lib.hpp"

namespace snabl::libs {
  struct Home: Lib {
    Home(Env &env);
    void init();
  };
}

#endif
