#include "snabl/env.hpp"
#include "snabl/state.hpp"

namespace snabl {
  State::State(const Env &env):
    lib(*env.task->lib),
    scope(env.task->scope),
    call(env.task->call),
    _try(env.task->_try),
    nstack(env.task->stack.size()),
    nsplits(env.task->splits.size()),
    async_depth(env.task->async_depth) { }

  void State::restore_env(Env &env) const {
    env.task->lib = &lib;
    env.task->scope = scope;
    auto &s(env.task->stack);
    while (I64(s.size()) > nstack) { s.erase(s.begin()+nstack, s.end()); }
    if (I64(env.task->splits.size()) > nsplits) { env.task->splits.resize(nsplits); }
    env.task->async_depth = async_depth;
  }

  void State::restore_call(Env &env) const { env.task->call = call; }

  void State::restore_try(Env &env) const { env.task->_try = _try; }
}
