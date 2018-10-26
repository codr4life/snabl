#ifndef SNABL_STATE_HPP
#define SNABL_STATE_HPP

namespace snabl {
  struct Call;
  struct Env;
  struct Lib;
  struct Scope;
  struct Try;
  
  struct State {
    Lib &lib;
    Scope *const scope;
    Call *const call;
    Try *const _try;
    const I64 nstack, nsplits, async_depth;

    State(const Env &env);
    void restore_env(Env &env) const;
    void restore_call(Env &env) const;
    void restore_try(Env &env) const;
  };
}

#endif
