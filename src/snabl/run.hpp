#ifndef SNABL_RUN_HPP
#define SNABL_RUN_HPP

#include "snabl/call.hpp"
#include "snabl/stack.hpp"

namespace snabl {
  class Env;
  
  struct RuntimeError: Error {
    RuntimeError(Env &env, Pos pos, const string &msg);
  };

  struct UserError: RuntimeError {
    const Box val;
    UserError(Env &env, Pos pos, const Box &_val);
  };
}

#endif
