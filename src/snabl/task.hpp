#ifndef SNABL_TASK_HPP
#define SNABL_TASK_HPP

#include "snabl/op.hpp"
#include "snabl/starray.hpp"

namespace snabl {
	class Env;

	struct Task;
	using TaskPtr = shared_ptr<Task>;

	struct Task {
		enum class Status {New, Running, Yielding, Done};
		static const Int MaxCalls = 8;
		static const Int MaxSplits = 8;
		static const Int MaxTries = 8;
		
		Task(const TaskPtr &next):
			_prev(nullptr), _next(next), _status(Status::New), _pc(nullptr) {
			if (next) {
				_prev = next->_prev;
				next->_prev = this;
				copy(next->_ops.begin(), next->_ops.end(), back_inserter(_ops));

				for (auto i(_ops.begin()), j = i+1; j != _ops.end(); i++, j++) {
					i->next = &*j;
				}
			}
		}
	private:
		Task *_prev;
		TaskPtr _next;
		Status _status;
		
		Ops _ops;
		PC _pc;

		Starray<Call, MaxCalls> _calls;
		Starray<ops::Try *, MaxTries> _tries;
		Starray<Int, MaxSplits> _splits;
		
		friend Env;
		friend State;
	};
}

#endif
