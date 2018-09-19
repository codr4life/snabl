#include "snabl/env.hpp"
#include "snabl/lambda.hpp"

namespace snabl {
	void Lambda::call(const LambdaPtr &l, Env &env, Pos pos, bool now) {
		if (l->_opts & Target::Opts::Vars) { env.begin_scope(l->_parent_scope); }
		
		if (now) {
			const auto prev_pc(env.pc);
			Target::begin_call(l, env, pos, nullptr);
			env.pc = &l->_start_pc;
			env.run();
			env.pc = prev_pc;
		} else {
			Target::begin_call(l, env, pos, env.pc);
			env.pc = &l->_start_pc;
		}
	}

}
