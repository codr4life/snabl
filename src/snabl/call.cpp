#include "snabl/env.hpp"
#include "snabl/fimp.hpp"
#include "snabl/func.hpp"
#include "snabl/call.hpp"

namespace snabl {	
	Call::Call(Scope &scope, Fimp &fimp, optional<PC> return_pc):
		scope(scope), state(scope.env), fimp(&fimp), lambda(nullptr),
		return_pc(return_pc) { }
	
	Call::Call(Scope &scope, Lambda &lambda, PC return_pc):
		scope(scope), state(scope.env), fimp(nullptr), lambda(&lambda),
		return_pc(return_pc) { }

	void Call::recall() {
		auto &env(scope.env);
		state.restore(env);
		scope.clear_vars();
		env.pc = env.ops.begin() + (fimp ? *fimp->start_pc() : lambda->start_pc);
	}
}
