#include "snabl/env.hpp"
#include "snabl/lambda.hpp"

namespace snabl {
	void Lambda::call(const LambdaPtr &l, Env &env, Pos pos, bool now) {
		if (l->_opts & Target::Opts::Vars) { env.begin_scope(l->_parent_scope); }
		
		if (now) {
			const auto prev_pc(env.pc());
			env.begin_call(l, pos, nullptr);
			env.jump(l->_start_pc);
			env.run();
			env.jump(prev_pc);
		} else {
			env.begin_call(l, pos, env.pc());
			env.jump(l->_start_pc);
		}
	}

}
