#ifndef SNABL_ERROR_HPP
#define SNABL_ERROR_HPP

#include <stdexcept>

#include "snabl/pos.hpp"
#include "snabl/std.hpp"

namespace snabl {
  struct Env;
  
  struct Error: runtime_error {
    Error(): runtime_error("") { }
    Error(const string &msg);
  };

  struct CompileError: Error {
    CompileError(Pos pos, const string &msg);
    CompileError(const string &msg);
  };

  struct SyntaxError: CompileError {
    SyntaxError(Pos pos, const string &msg);
  };

  struct RuntimeError: Error {
    RuntimeError(Env &env, Pos pos, const string &msg);
  };
}

#endif
