#ifndef SNABL_ERROR_HPP
#define SNABL_ERROR_HPP

#include <stdexcept>

#include "snabl/fmt.hpp"
#include "snabl/pos.hpp"
#include "snabl/std.hpp"

namespace snabl {
  struct Error: runtime_error {
    string msg;
    
    Error(const string &msg=""): runtime_error(""), msg(msg) { }

    const char* what() const throw() override {
      if (msg.empty()) { const_cast<Error *>(this)->msg = generate_msg(); }
      return msg.c_str();
    }

    virtual string generate_msg() const { return "Undefined error"; }
  };

  struct CompileError: Error {
    const string compile_msg;
    const Pos pos;
    
    CompileError(Pos pos, const string &msg): compile_msg(msg), pos(pos) { }

    string generate_msg() const override {
      return fmt("Compile error in row %0, col %1: %2",
                 {pos.row, pos.col, compile_msg});
    }
  };

  struct SyntaxError: CompileError {
    SyntaxError(Pos pos, const string &msg): CompileError(pos, msg) { }

    string generate_msg() const override {
      return fmt("Syntax error in row %0, col %1: %2",
                 {pos.row, pos.col, compile_msg});
    }
  };

  inline bool operator ==(const Error &lhs, const Error &rhs) {
    return false;
  }

  inline bool operator <(const Error &lhs, const Error &rhs) {
    throw Error("Errors are not comparable");
  }
}

#endif
