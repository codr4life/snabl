#ifndef SNABL_STATE_HPP
#define SNABL_STATE_HPP

#include "snabl/ptrs.hpp"

namespace snabl {
  struct Env;
  struct Lib;

  namespace ops { struct Try; }
  
  struct State {
    Lib &lib;
    const ScopePtr scope;
    const I64 ncalls, ntries, nstack, nsplits, async_depth;

    State(const Env &env);
    void restore_env(Env &env) const;
    void restore_calls(Env &env) const;
    void restore_tries(Env &env) const;
  };
}

#endif
