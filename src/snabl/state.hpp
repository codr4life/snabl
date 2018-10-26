#ifndef SNABL_STATE_HPP
#define SNABL_STATE_HPP

namespace snabl {
  struct Call;
  struct Env;
  struct Lib;
  struct Scope;
  
  namespace ops { struct Try; }
  
  struct State {
    Lib &lib;
    Scope *const scope;
    Call *const call;
    const I64 ntries, nstack, nsplits, async_depth;

    State(const Env &env);
    void restore_env(Env &env) const;
    void restore_call(Env &env) const;
    void restore_tries(Env &env) const;
  };
}

#endif
