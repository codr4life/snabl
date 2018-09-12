#ifndef SNABL_TYPE_LAMBDA_HPP
#define SNABL_TYPE_LAMBDA_HPP

#include "snabl/op.hpp"
#include "snabl/ptrs.hpp"
#include "snabl/std.hpp"
#include "snabl/target.hpp"
#include "snabl/type.hpp"

namespace snabl {
	struct Lambda: public Target {
		const ScopePtr parent_scope;
		const size_t nops;
		const bool has_vars;

		Lambda(const ScopePtr &parent_scope,
					 size_t start_pc, size_t nops,
					 bool has_vars):
			parent_scope(parent_scope),
			nops(nops),
			has_vars(has_vars),
			_start_pc(start_pc) { }

		size_t start_pc() const override { return _start_pc; }
	private:
		const size_t _start_pc;

		friend bool operator ==(const Lambda &, const Lambda &);
		friend bool operator <(const Lambda &, const Lambda &);
	};

	inline bool operator ==(const Lambda &lhs, const Lambda &rhs) {
		return lhs._start_pc == rhs._start_pc;
	}

	inline bool operator <(const Lambda &lhs, const Lambda &rhs) {
		return lhs._start_pc < rhs._start_pc;
	}

	class LambdaType: public Type<LambdaPtr> {
	public:
		LambdaType(Lib &lib, Sym id);
		void call(const Box &val, Pos pos, bool now) const override;
		void dump(const Box &val, ostream &out) const override;
	};
}

#endif
