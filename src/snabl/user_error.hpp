#ifndef SNABL_USER_ERROR_HPP
#define SNABL_USER_ERROR_HPP

#include "snabl/runtime_error.hpp"

namespace snabl {
  struct UserError: RuntimeError {
    const Box val;
    
    UserError(Env &env, const Box &val): RuntimeError(env, ""), val(val) { }

    string generate_msg() const override {
      stringstream buf;
      buf << val;
      return buf.str();
    }
  };
}

#endif
