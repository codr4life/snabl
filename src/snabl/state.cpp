#include "snabl/env.hpp"
#include "snabl/state.hpp"

namespace snabl {
	State::State(const Env &env):
		_lib(*env._lib),
		_target(env._target),
		_try(env._try),
		_nscopes(env._scopes.size()),
		_nstack(env._stack.size()),
		_nsplits(env._splits.size()) { }

	void State::restore_lib(Env &env) const { env._lib = &_lib; }

	void State::restore_target(Env &env) const {
		while (env._target != _target) { env._target->end_call(env); }
	}

	void State::restore_try(Env &env) const { env._try = _try; }

	void State::restore_scopes(Env &env) const {
		if (env._scopes.size() > _nscopes) { env._scopes.resize(_nscopes); }
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
