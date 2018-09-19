#include "snabl/env.hpp"
#include "snabl/lambda.hpp"

namespace snabl {
	void Lambda::call(const LambdaPtr &l, Env &env, Pos pos, bool now) {
		if (l->_opts & Target::Opts::Vars) { env.begin_scope(l->parent_scope); }
		l->begin_call(env);
		
		if (now) {
			const auto prev_pc(env.pc);
			env.begin_call(pos, l, nullptr);
			env.pc = &l->_start_pc;
			env.run();
			env.pc = prev_pc;
		} else {
			env.begin_call(pos, l, env.pc);
			env.pc = &l->_start_pc;
		}
	}

}
