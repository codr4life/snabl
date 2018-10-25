#ifndef SNABL_RUNTIME_ERROR_HPP
#define SNABL_RUNTIME_ERROR_HPP

#include "snabl/error.hpp"
#include "snabl/stack.hpp"

namespace snabl {
  struct RuntimeError: Error {
    const string run_msg;
    const Pos pos;
    const Stack stack;
    
    RuntimeError(Env &env, const string &msg);

    string generate_msg() const override {
      stringstream buf;
      
      buf << stack << endl
          << "Error in row " << pos.row << ", col " << pos.col << ":\n"
          << msg;
      
      return buf.str();
    }
  };
}

#endif
