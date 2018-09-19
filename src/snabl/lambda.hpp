#ifndef SNABL_LAMBDA_HPP
#define SNABL_LAMBDA_HPP

#include "snabl/pos.hpp"
#include "snabl/ptrs.hpp"
#include "snabl/std.hpp"
#include "snabl/target.hpp"

namespace snabl {
	struct Lambda: public Target {
		static void call(const LambdaPtr &l, Env &env, Pos pos, bool now);

		Lambda(const ScopePtr &parent_scope,
					 const OpImp &start_pc, const OpImp &end_pc,
					 Opts opts):
			Target(parent_scope, start_pc, end_pc, opts) { }
	private:
		friend ops::LambdaEnd;

		friend bool operator ==(const Lambda &, const Lambda &);
		friend bool operator <(const Lambda &, const Lambda &);
	};

	inline bool operator ==(const Lambda &lhs, const Lambda &rhs) {
		return &lhs == &rhs;
	}

	inline bool operator <(const Lambda &lhs, const Lambda &rhs) {
		return &lhs < &rhs;
	}
}

#endif
