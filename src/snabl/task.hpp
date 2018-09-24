#ifndef SNABL_TASK_HPP
#define SNABL_TASK_HPP

#include "snabl/op.hpp"
#include "snabl/state.hpp"

namespace snabl {
	class Env;

	struct Task;
	using TaskPtr = shared_ptr<Task>;

	struct Task {
		Task(const TaskPtr &next): _next(next), _pc(nullptr) { }
	private:
		TaskPtr _next;
		
		optional<State> _state;
		Ops _ops;
		PC _pc;

		friend Env;
	};
}

#endif
