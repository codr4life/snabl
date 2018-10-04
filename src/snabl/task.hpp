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
		Stack _stack;
	public:
		enum class Status {New, Running, Yielding, Done};
		static const Int MaxCalls = 8;
		static const Int MaxSplits = 8;
		static const Int MaxTries = 8;
		
		Task(Env &env, PC start_pc, const ScopePtr &parent_scope);
		
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
		TaskPtr _prev, _next;
		Status _status;
		Int _stack_offs;
		Lib *_lib;
		PC _pc;
		
		Starray<Call, MaxCalls> _calls;
		Sarray<ops::Try *, MaxTries> _tries;
		Sarray<Int, MaxSplits> _splits;
		
		friend Env;
		friend RuntimeError;
		friend State;

		friend ops::DDrop::Type;
		friend ops::Drop::Type;
		friend ops::Dup::Type;
		friend ops::Eqval::Type;
		friend ops::Fimp::Type;
		friend ops::Funcall::Type;
		friend ops::Isa::Type;
		friend ops::Let::Type;
		friend ops::Recall::Type;
		friend ops::Return::Type;
		friend ops::Rot::Type;
		friend ops::RSwap::Type;
		friend ops::SDrop::Type;
		friend ops::Stack::Type;
		friend ops::Swap::Type;
		friend ops::Task::Type;
		friend ops::Try::Type;
		friend ops::TryEnd::Type;		
	};
}

#endif
