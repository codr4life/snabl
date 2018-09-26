#include "snabl/env.hpp"
#include "snabl/state.hpp"

namespace snabl {
	State::State(const Env &env):
		_lib(*env._lib),
		_scope(env._scope),
		_try(env._try),
		_ncalls(env._task->_calls.size()),
		_nstack(env._stack.size()),
		_nsplits(env._splits.size()) { }

	void State::restore_lib(Env &env) const { env._lib = &_lib; }

	void State::restore_scope(Env &env) const { env._scope = _scope; }

	void State::restore_try(Env &env) const { env._try = _try; }

	void State::restore_calls(Env &env) const {
		auto &calls(env._task->_calls);
		while (Int(calls.size()) > _ncalls) { calls.pop_back(); }
	}
	
	void State::restore_stack(Env &env) const {
		if (Int(env._stack.size()) > _nstack) {
			env._stack.erase(env._stack.begin()+_nstack, env._stack.end());
		}
	}

	void State::restore_splits(Env &env) const {
		if (Int(env._splits.size()) > _nsplits) { env._splits.resize(_nsplits); }
	}
}
