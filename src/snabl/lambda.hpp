#ifndef SNABL_LAMBDA_HPP
#define SNABL_LAMBDA_HPP

#include "snabl/ptrs.hpp"
#include "snabl/std.hpp"
#include "snabl/target.hpp"

namespace snabl {
	struct Lambda: public Target {
		const ScopePtr parent_scope;
		const size_t nops;

		Lambda(const ScopePtr &parent_scope,
					 size_t start_pc, size_t nops,
					 Opts opts):
			parent_scope(parent_scope),
			nops(nops),
			_opts(opts),
			_start_pc(start_pc) { }

		Opts opts() const override { return _opts; }
		size_t start_pc() const override { return _start_pc; }
	private:
		const Opts _opts;
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
}

#endif
