#ifndef SNABL_REPL_HPP
#define SNABL_REPL_HPP

#include "snabl/std.hpp"

namespace snabl { 
  struct Env;

  void repl(Env &env, istream &in, ostream &out);
}

#endif
