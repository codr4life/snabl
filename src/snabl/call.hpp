#ifndef SNABL_CALL_HPP
#define SNABL_CALL_HPP

#include "snabl/bin.hpp"
#include "snabl/ptrs.hpp"
#include "snabl/std.hpp"
#include "snabl/scope.hpp"
#include "snabl/target.hpp"

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
