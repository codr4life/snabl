#include "snabl/env.hpp"
#include "snabl/state.hpp"

namespace snabl {
	State::State(const Env &env):
		_lib(*env._lib),
		_scope(env._scope),
		_try(env._try),
		_target(env._target),
		_nstack(env._stack.size()),
		_nsplits(env._splits.size()) { }

	void State::restore_lib(Env &env) const { env._lib = &_lib; }

	void State::restore_scope(Env &env) const { env._scope = _scope; }

	void State::restore_try(Env &env) const { env._try = _try; }

	void State::restore_target(Env &env) const {
		while (env._target != _target) { env._target->end_call(env); }
	}
	
	void State::restore_stack(Env &env) const {
		if (env._stack.size() > _nstack) {
			env._stack.erase(env._stack.begin()+_nstack, env._stack.end());
		}
	}

	void State::restore_splits(Env &env) const {
		if (env._splits.size() > _nsplits) { env._splits.resize(_nsplits); }
	}
}
