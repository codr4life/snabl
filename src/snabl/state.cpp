#include "snabl/env.hpp"
#include "snabl/state.hpp"

namespace snabl {
  State::State(const Env &env):
    lib(*env.task->lib),
    scope(env.task->scope),
    ncalls(env.task->calls.size()),
    ntries(env.task->tries.size()),
    nstack(env.task->stack.size()),
    nsplits(env.task->splits.size()) { }

  void State::restore_lib(Env &env) const { env.task->lib = &lib; }

  void State::restore_scope(Env &env) const { env.task->scope = scope; }

  void State::restore_calls(Env &env) const {
    auto &calls(env.task->calls);
    if (calls.size() > ncalls) { calls.trunc(ncalls); }
  }
  
  void State::restore_tries(Env &env) const {
    if (env.task->tries.size() > ntries) { env.task->tries.trunc(ntries); }
  }

  void State::restore_stack(Env &env) const {
    auto &s(env.task->stack);
    if (Int(s.size()) > nstack) { s.erase(s.begin()+nstack, s.end()); }
  }

  void State::restore_splits(Env &env) const {
    if (env.task->splits.size() > nsplits) { env.task->splits.trunc(nsplits); }
  }
}
