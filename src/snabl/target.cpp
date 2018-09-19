#include "snabl/env.hpp"
#include "snabl/target.hpp"

namespace snabl {
	void Target::begin_call(Env &env) {
		if (_opts & Target::Opts::Recalls) { _prev_state.emplace(env); }
	}

	void Target::recall(Env &env) const {
		_prev_state->restore_libs(env);
		_prev_state->restore_scopes(env);
		if (_opts & Target::Opts::Vars) { env.scope()->clear_vars(); }
		env.pc = &_start_pc;
	}
}
