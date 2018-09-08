#include "snabl/env.hpp"
#include "snabl/state.hpp"

namespace snabl {
	State::State(const Env &env):
		nlibs(env._libs.size()),
		nscopes(env._scopes.size()) { }

	void State::restore(Env &env) const {
		if (env._libs.size() > nlibs) { env._libs.resize(nlibs); }
		if (env._scopes.size() > nscopes) { env._scopes.resize(nscopes); }
	}
}
