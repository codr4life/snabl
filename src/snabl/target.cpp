#include "snabl/env.hpp"
#include "snabl/target.hpp"

namespace snabl {
	void Target::begin_call(const TargetPtr &t, Env &env, Pos pos, PC return_pc) {
		if (t->_call) {
			throw RuntimeError(env, pos, fmt("Recursive call: %0", {t->target_id()}));
		}

		t->_call.emplace(env._target,
										 pos,
										 (t->_opts & Target::Opts::Recalls)
										   ? make_optional<State>(env)
										   : nullopt,
										 return_pc);
		
		env._target = t;
	}

	void Target::end_call(Env &env) {
		_call.reset();
		env._target = _call->parent;
	}

	void Target::recall(Env &env) const {
		_call->state->restore_libs(env);
		_call->state->restore_scopes(env);
		if (_opts & Target::Opts::Vars) { env.scope()->clear_vars(); }
		env.pc = &_start_pc;
	}
}
