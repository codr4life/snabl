#include "snabl/env.hpp"
#include "snabl/task.hpp"

namespace snabl {
	Task::Task(Env &env, PC start_pc, const ScopePtr &parent_scope):
		root_scope(begin_scope(parent_scope)),
		_status(Status::New),
		_stack_offs(0),
		_lib(&env.home_lib),
		_pc(start_pc) { }
}
