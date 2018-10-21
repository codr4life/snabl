#include "snabl/env.hpp"
#include "snabl/state.hpp"

namespace snabl {
  State::State(const Env &env):
    lib(*env.task->lib),
    scope(env.task->scope),
    ncalls(env.task->calls.size),
    ntries(env.task->tries.size),
    nstack(env.task->stack.size()),
    nsplits(env.task->splits.size),
    async_depth(env.task->async_depth) { }

  void State::restore_env(Env &env) const {
    env.task->lib = &lib;
    env.task->scope = scope;
    auto &s(env.task->stack);
    while (I64(s.size()) > nstack) { s.erase(s.begin()+nstack, s.end()); }
    if (env.task->splits.size > nsplits) { env.task->splits.trunc(nsplits); }
    env.task->async_depth = async_depth;
  }

  void State::restore_calls(Env &env) const {
    auto &calls(env.task->calls);
    if (calls.size > ncalls) { calls.trunc(ncalls); }
  }

  void State::restore_tries(Env &env) const {
    if (env.task->tries.size > ntries) { env.task->tries.trunc(ntries); }
  }
  
}
