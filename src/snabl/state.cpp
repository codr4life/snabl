#include "snabl/env.hpp"
#include "snabl/state.hpp"

namespace snabl {
  State::State(const Env &env):
    _lib(*env.task->lib),
    _scope(env.task->scope),
    _ncalls(env.task->calls.size()),
    _ntries(env.task->tries.size()),
    _nstack(env.task->stack.size()),
    _nsplits(env.task->splits.size()) { }

  void State::restore_lib(Env &env) const { env.task->lib = &_lib; }

  void State::restore_scope(Env &env) const { env.task->scope = _scope; }

  void State::restore_calls(Env &env) const {
    auto &calls(env.task->calls);
    if (calls.size() > _ncalls) { calls.trunc(_ncalls); }
  }
  
  void State::restore_tries(Env &env) const {
    if (env.task->tries.size() > _ntries) { env.task->tries.trunc(_ntries); }
  }

  void State::restore_stack(Env &env) const {
    auto &s(env.task->stack);
    if (Int(s.size()) > _nstack) { s.erase(s.begin()+_nstack, s.end()); }
  }

  void State::restore_splits(Env &env) const {
    if (env.task->splits.size() > _nsplits) { env.task->splits.trunc(_nsplits); }
  }
}
