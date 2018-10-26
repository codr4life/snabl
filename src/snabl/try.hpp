#ifndef SNABL_TRY_HPP
#define SNABL_TRY_HPP

#include "snabl/state.hpp"

namespace snabl {
  struct Env;
  namespace ops { struct Try; }

  struct Try {
    Try *prev;
    ops::Try &op;
    State state;

    Try(Env &env, ops::Try &op);
  };
}

#endif
