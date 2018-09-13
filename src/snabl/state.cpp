#include "snabl/env.hpp"
#include "snabl/state.hpp"

namespace snabl {
	State::State(const Env &env):
		nlibs(env._libs.size()),
		nscopes(env._scopes.size()),
		ncalls(env._calls.size()),
		nstack(env._stack.size()),
		nsplits(env._splits.size()) { }

	void State::restore_all(Env &env) const {
		restore_libs(env);
		restore_scopes(env);
		restore_calls(env);
		restore_stack(env);
		restore_splits(env);
	}

	void State::restore_libs(Env &env) const {
		if (env._libs.size() > nlibs) { env._libs.resize(nlibs); }
	}

	void State::restore_scopes(Env &env) const {
		if (env._scopes.size() > nscopes) { env._scopes.resize(nscopes); }
	}

	void State::restore_calls(Env &env) const {
		while (env._calls.size() > ncalls) { env._calls.pop_back(); }
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
