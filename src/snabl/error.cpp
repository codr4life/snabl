#include "snabl/env.hpp"
#include "snabl/fmt.hpp"
#include "snabl/pos.hpp"
#include "snabl/error.hpp"

namespace snabl {
  Error::Error(const string &msg): runtime_error(msg) { }

  CompileError::CompileError(Pos pos, const string &msg):
    Error(fmt("Compile error in row %0, col %1: %2", {pos.row, pos.col, msg})) { }

  CompileError::CompileError(const string &msg): Error(msg) { }

  SyntaxError::SyntaxError(Pos pos, const string &msg):
    CompileError(fmt("Syntax error in row %0, col %1: %2",
                     {pos.row, pos.col, msg})) { }

  static string runtime_str(Env &env, const string &msg) {
    const auto pos(env.pos());
    stringstream buf;
        
    buf << env.task->stack << endl
        << "Error in row " << pos.row << ", col " << pos.col << ":\n"
        << msg;
    
    return buf.str();
  }
  
  RuntimeError::RuntimeError(Env &env, const string &msg):
    Error(runtime_str(env, msg)) { }
}
