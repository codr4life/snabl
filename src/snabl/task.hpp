#ifndef SNABL_TASK_HPP
#define SNABL_TASK_HPP

#include "snabl/op.hpp"
#include "snabl/sarray.hpp"
#include "snabl/starray.hpp"

namespace snabl {
	class Env;

	struct Task;
	using TaskPtr = shared_ptr<Task>;

	struct Task {
	private:
		ScopePtr _scope;
	public:
		enum class Status {New, Running, Yielding, Done};
		static const Int MaxCalls = 8;
		static const Int MaxSplits = 8;
		static const Int MaxTries = 8;
		const ScopePtr &root_scope;
		
		Task(Env &env, const TaskPtr &next):
			root_scope(begin_scope()),
			_prev(nullptr), _next(next), _status(Status::New), _pc(nullptr) {
			if (next) {
				_prev = next->_prev;
				next->_prev = this;
			}
		}

		const ScopePtr &begin_scope(const ScopePtr &parent=nullptr) {
			_scope = make_shared<Scope>(_scope, parent);
			return _scope;
		}

		void end_scope() {
			auto prev(_scope->prev);
			_scope->prev = nullptr;
			_scope = prev;
		}
	private:
		Task *_prev;
		TaskPtr _next;
		Status _status;		
		PC _pc;

		Starray<Call, MaxCalls> _calls;
		Sarray<ops::Try *, MaxTries> _tries;
		Sarray<Int, MaxSplits> _splits;
		
		friend Env;
		friend State;
	};
}

#endif
