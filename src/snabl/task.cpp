#include "snabl/env.hpp"
#include "snabl/task.hpp"

namespace snabl {
  Task::Task(Env &env, PC start_pc, const ScopePtr &parent_scope):
    scope(parent_scope), prev(this), next(this), pc(start_pc) { }

  const ScopePtr &Task::begin_scope(Env &env, const ScopePtr &parent) {
    scope = ScopePtr::make(&env.scope_pool, scope, parent);
    return scope;
  }

  void Task::end_scope() {
    auto prev(scope->prev);
    scope->prev = nullptr;
    scope = prev;
  }
}
