#include <ostream>

#include "snabl/env.hpp"
#include "snabl/box.hpp"
#include "snabl/lib.hpp"
#include "snabl/types/lambda.hpp"

namespace snabl {
	Lambda::Lambda(const ScopePtr &parent_scope, Ops::iterator start_pc, size_t nops):
		parent_scope(parent_scope), start_pc(start_pc), nops(nops) { }

	bool operator ==(const Lambda &lhs, const Lambda &rhs) {
		return lhs.start_pc == rhs.start_pc;
	}

	bool operator <(const Lambda &lhs, const Lambda &rhs) {
		return lhs.start_pc < rhs.start_pc;
	}

	LambdaType::LambdaType(Lib &lib, Sym id): Type<Lambda>(lib, id) { }

	void LambdaType::call(const Box &val, bool now) const {
		const Lambda l(val.as<Lambda>());
		Env &env(lib.env);
		auto scope(env.begin_scope(l.parent_scope));
		auto ret_pc(env.pc);
		env.begin_call(l, ret_pc);
		env.pc = l.start_pc;
		if (now) { env.run(ret_pc); }
	}

	void LambdaType::dump(const Box &val, ostream &out) const {
		auto l(val.as<Lambda>());
		out << "Lambda(" << l.start_pc-val.type()->lib.env.ops.begin() << ')';
	}
}
