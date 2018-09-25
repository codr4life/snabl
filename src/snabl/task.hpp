#ifndef SNABL_TASK_HPP
#define SNABL_TASK_HPP

#include "snabl/op.hpp"
#include "snabl/state.hpp"

namespace snabl {
	class Env;

	struct Task;
	using TaskPtr = shared_ptr<Task>;

	struct Task {
		enum class State {New, Running, Yielding, Done};
		
		Task(const TaskPtr &next):
			_prev(nullptr), _next(next), _pc(nullptr), _state(State::New) {
			if (next) {
				_prev = next->_prev;
				next->_prev = this;
				copy(next->_ops.begin(), next->_ops.end(), back_inserter(_ops));
			}
		}
	private:
		Task *_prev;
		TaskPtr _next;
		
		Ops _ops;
		PC _pc;

		State _state;
		friend Env;
	};
}

#endif
