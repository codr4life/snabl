#include "snabl/env.hpp"
#include "snabl/task.hpp"

namespace snabl {
  Task::Task(Env &env, PC start_pc, const ScopePtr &parent_scope):
    scope(parent_scope),
    status(Status::New),
    stack_offs(0),
    lib(&env.home_lib),
    pc(start_pc) { }
}
