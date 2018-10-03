#include "snabl/env.hpp"
#include "snabl/task.hpp"

namespace snabl {
	Task::Task(Env &env,
						 const TaskPtr &next,
						 PC start_pc,
						 const ScopePtr &parent_scope):
		root_scope(begin_scope(parent_scope)),
		_prev(nullptr),
		_next(next),
		_status(Status::New),
		_stack_offs(0),
		_lib(&env.home_lib),
		_pc(start_pc) {
		if (next) {
			_prev = next->_prev;
			next->_prev = this;
		}
	}
}
