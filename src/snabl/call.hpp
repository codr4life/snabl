#ifndef SNABL_CALL_HPP
#define SNABL_CALL_HPP

#include "snabl/scope.hpp"
#include "snabl/target.hpp"

namespace snabl {
	class Call {
	public:
		const TargetPtr target;
		const ScopePtr scope;
		const ssize_t return_pc;
		
		Call(const TargetPtr &target, const ScopePtr &scope, ssize_t return_pc);
	};
}

#endif
