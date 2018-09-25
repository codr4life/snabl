#ifndef SNABL_TASK_HPP
#define SNABL_TASK_HPP

#include "snabl/op.hpp"

namespace snabl {
	class Env;

	struct Task;
	using TaskPtr = shared_ptr<Task>;

	struct Task {
		enum class Status {New, Running, Yielding, Done};
		
		Task(const TaskPtr &next):
			_prev(nullptr), _next(next), _status(Status::New), _pc(nullptr) {
			if (next) {
				_prev = next->_prev;
				next->_prev = this;
				copy(next->_ops.begin(), next->_ops.end(), back_inserter(_ops));
			}
		}
	private:
		Task *_prev;
		TaskPtr _next;
		Status _status;
		
		Ops _ops;
		PC _pc;

		vector<Call> _calls;
		
		friend Env;
		friend State;
	};
}

#endif
