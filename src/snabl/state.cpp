#include "snabl/env.hpp"
#include "snabl/state.hpp"

namespace snabl {
	State::State(const Env &env):
		_lib(*env._lib),
		_scope(env._task->_scope),
		_ncalls(env._task->_calls.size()),
		_ntries(env._task->_tries.size()),
		_nstack(env._task->_stack.size()),
		_nsplits(env._task->_splits.size()) { }

	void State::restore_lib(Env &env) const { env._lib = &_lib; }

	void State::restore_scope(Env &env) const { env._task->_scope = _scope; }

	void State::restore_calls(Env &env) const {
		auto &calls(env._task->_calls);
		if (calls.size() > _ncalls) { calls.trunc(_ncalls); }
	}
	
	void State::restore_tries(Env &env) const {
		if (env._task->_tries.size() > _ntries) { env._task->_tries.trunc(_ntries); }
	}

	void State::restore_stack(Env &env) const {
		auto &s(env._task->_stack);
		if (Int(s.size()) > _nstack) { s.erase(s.begin()+_nstack, s.end()); }
	}

	void State::restore_splits(Env &env) const {
		if (env._task->_splits.size() > _nsplits) { env._task->_splits.trunc(_nsplits); }
	}
}
