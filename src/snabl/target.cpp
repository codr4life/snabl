#include "snabl/env.hpp"
#include "snabl/target.hpp"

namespace snabl {
	void Target::begin_call(const TargetPtr &tp, Env &env, Pos pos, PC return_pc) {
		auto &t(*tp);
		
		if (t._call) {
			throw RuntimeError(env, pos, fmt("Recursive call: %0", {t.target_id()}));
		}

		t._call.emplace(env._target,
										 pos,
										 (t._opts & Target::Opts::Recalls)
										   ? make_optional<State>(env)
										   : nullopt,
										 return_pc);
		
		env._target = tp;
	}

	void Target::end_call(Env &env) {
		const auto p(_call->parent);
		_call.reset();
		env._target = p;
	}

	void Target::recall(Env &env) const {
		_call->state->restore_lib(env);
		_call->state->restore_scopes(env);
		if (_opts & Target::Opts::Vars) { env.scope()->clear_vars(); }
		env.pc = &_start_pc;
	}
}
