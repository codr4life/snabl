#include "snabl/env.hpp"
#include "snabl/fimp.hpp"
#include "snabl/func.hpp"
#include "snabl/call.hpp"

namespace snabl {	
	void Call::recall() const {
		auto &env(scope.env);
		_state->restore_libs(env);
		_state->restore_scopes(env);
		scope.clear_vars();
		env.pc = env.ops.begin() + target->start_pc();
	}
}
