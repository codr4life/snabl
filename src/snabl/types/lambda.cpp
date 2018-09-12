#include <ostream>

#include "snabl/call.hpp"
#include "snabl/env.hpp"
#include "snabl/box.hpp"
#include "snabl/lib.hpp"
#include "snabl/types/lambda.hpp"

namespace snabl {
	LambdaType::LambdaType(Lib &lib, Sym id): Type<LambdaPtr>(lib, id) { }

	void LambdaType::call(const Box &val, Pos pos, bool now) const {
		const auto l(val.as<LambdaPtr>());
		Env &env(lib.env);
		auto &scope((l->opts() & Target::Opts::Vars)
								? env.begin_scope(l->parent_scope)
								: env.scope());
		auto &call(env.begin_call(*scope, pos, l, env.pc));
		env.pc = env.ops.begin()+l->start_pc();
		if (now) { env.run(*call.return_pc); }
	}

	void LambdaType::dump(const Box &val, ostream &out) const {
		auto l(val.as<LambdaPtr>());
		out << "Lambda(" << l->start_pc() << ')';
	}
}
