#ifndef SNABL_LAMBDA_HPP
#define SNABL_LAMBDA_HPP

#include "snabl/op.hpp"
#include "snabl/ptrs.hpp"
#include "snabl/std.hpp"
#include "snabl/type.hpp"

namespace snabl {
	struct Lambda {
		const ScopePtr parent_scope;
		const Ops::iterator start_pc;
		const size_t nops;
		
		Lambda(const ScopePtr &parent_scope, Ops::iterator start_pc, size_t nops);
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
