#ifndef SNABL_TYPE_LAMBDA_HPP
#define SNABL_TYPE_LAMBDA_HPP

#include "snabl/op.hpp"
#include "snabl/ptrs.hpp"
#include "snabl/std.hpp"
#include "snabl/type.hpp"

namespace snabl {
	struct Lambda {
		const ScopePtr parent_scope;
		const size_t start_pc, nops;
		const bool has_vars;
		
		Lambda(const ScopePtr &parent_scope, size_t start_pc, size_t nops, bool has_vars);
	};

	bool operator ==(const Lambda &lhs, const Lambda &rhs);
	bool operator <(const Lambda &lhs, const Lambda &rhs);

	class LambdaType: public Type<LambdaPtr> {
	public:
		LambdaType(Lib &lib, Sym id);
		void call(const Box &val, bool now) const override;
		void dump(const Box &val, ostream &out) const override;
	};
}

#endif
