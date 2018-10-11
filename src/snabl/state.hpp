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
    const I64 ncalls, ntries, nstack, nsplits;

    State(const Env &env);
    void restore_lib(Env &env) const;
    void restore_scope(Env &env) const;
    void restore_tries(Env &env) const;
    void restore_calls(Env &env) const;
    void restore_stack(Env &env) const;
    void restore_splits(Env &env) const;
  };
}

#endif
