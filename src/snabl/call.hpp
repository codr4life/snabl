#ifndef SNABL_CALL_HPP
#define SNABL_CALL_HPP

#include "snabl/bin.hpp"
#include "snabl/std.hpp"
#include "snabl/scope.hpp"
#include "snabl/target.hpp"

namespace snabl {
	class Call {
	public:
		const TargetPtr target;
		const ScopePtr scope;
		const optional<PC> return_pc;
		
		Call(const TargetPtr &target,
				 const ScopePtr &scope,
				 optional<PC> return_pc=nullopt);
	};
}

#endif
