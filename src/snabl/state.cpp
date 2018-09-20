#include "snabl/env.hpp"
#include "snabl/state.hpp"

namespace snabl {
	State::State(const Env &env):
		lib(*env._lib),
		target(env._target),
		nscopes(env._scopes.size()),
		nstack(env._stack.size()),
		nsplits(env._splits.size()) { }

	void State::restore_all(Env &env) const {
		restore_lib(env);
		restore_scopes(env);
		restore_target(env);
		restore_stack(env);
		restore_splits(env);
	}

	void State::restore_lib(Env &env) const { env._lib = &lib; }

	void State::restore_scopes(Env &env) const {
		if (env._scopes.size() > nscopes) { env._scopes.resize(nscopes); }
	}

	void State::restore_target(Env &env) const {
		while (env._target != target) { env._target->end_call(env); }
	}

	void State::restore_stack(Env &env) const {
		if (env._stack.size() > nstack) {
			env._stack.erase(env._stack.begin()+nstack, env._stack.end());
		}
	}

	void State::restore_splits(Env &env) const {
		if (env._splits.size() > nsplits) { env._splits.resize(nsplits); }
	}
}
