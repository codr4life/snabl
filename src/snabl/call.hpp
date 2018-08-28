#ifndef SNABL_CALL_HPP
#define SNABL_CALL_HPP

#include "snabl/op.hpp"
#include "snabl/ptrs.hpp"
#include "snabl/std.hpp"
#include "snabl/scope.hpp"

namespace snabl {
	class CallTarget {
	public:
		virtual void dump(ostream &out) const=0;
	};

	class Call {
	public:
		const CallTargetPtr target;
		const ScopePtr scope;
		const optional<PC> return_pc;
		
		Call(const CallTargetPtr &target,
				 const ScopePtr &scope,
				 optional<PC> return_pc=nullopt);
	};
}

#endif
