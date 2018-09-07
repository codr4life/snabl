#include <ostream>

#include "snabl/call.hpp"
#include "snabl/env.hpp"
#include "snabl/box.hpp"
#include "snabl/lib.hpp"
#include "snabl/types/lambda.hpp"

namespace snabl {
	Lambda::Lambda(const ScopePtr &parent_scope,
								 size_t start_pc, size_t nops,
								 bool has_vars):
		parent_scope(parent_scope), start_pc(start_pc), nops(nops), has_vars(has_vars) { }

	bool operator ==(const Lambda &lhs, const Lambda &rhs) {
		return lhs.start_pc == rhs.start_pc;
	}

	bool operator <(const Lambda &lhs, const Lambda &rhs) {
		return lhs.start_pc < rhs.start_pc;
	}

	LambdaType::LambdaType(Lib &lib, Sym id): Type<LambdaPtr>(lib, id) { }

	void LambdaType::call(const Box &val, bool now) const {
		const auto l(val.as<LambdaPtr>());
		Env &env(lib.env);
		auto &scope(l->has_vars ? env.begin_scope(l->parent_scope) : env.scope());
		auto &call(env.begin_call(*scope, *l, env.pc));
		env.pc = env.ops.begin()+l->start_pc;
		if (now) { env.run(*call.return_pc); }
	}

	void LambdaType::dump(const Box &val, ostream &out) const {
		auto l(val.as<LambdaPtr>());
		out << "Lambda(" << l->start_pc << ')';
	}
}
