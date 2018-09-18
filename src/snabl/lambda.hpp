#ifndef SNABL_LAMBDA_HPP
#define SNABL_LAMBDA_HPP

#include "snabl/ptrs.hpp"
#include "snabl/std.hpp"
#include "snabl/target.hpp"

namespace snabl {
	struct Lambda: public Target {
		const ScopePtr parent_scope;
		const OpImp &end_pc;

		Lambda(const ScopePtr &parent_scope,
					 OpImp &start_pc, OpImp &end_pc,
					 Opts opts):
			parent_scope(parent_scope),
			end_pc(end_pc),
			_opts(opts),
			_start_pc(start_pc) { }

		Opts opts() const override { return _opts; }
		PC start_pc() const override { return &_start_pc; }
	private:
		const Opts _opts;
		const OpImp &_start_pc;

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
