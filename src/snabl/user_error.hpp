#ifndef SNABL_USER_ERROR_HPP
#define SNABL_USER_ERROR_HPP

#include "snabl/box.hpp"
#include "snabl/error.hpp"

namespace snabl {
  struct UserError: RuntimeError {
    const Box val;

    static string val_str(const Box &val) {
      stringstream buf;
      buf << val;
      return buf.str();
    }

    UserError(Env &env, const Box &_val):
      RuntimeError(env, val_str(_val)), val(_val) { }
  };
}

#endif
