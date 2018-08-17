#ifndef SNABL_CALL_HPP
#define SNABL_CALL_HPP

#include "snabl/fimp.hpp"
#include "snabl/scope.hpp"
#include "snabl/target.hpp"

namespace snabl {
	class Call {
	public:
		const TargetPtr target;
		const ScopePtr scope;
		
		Call(const TargetPtr &target, const ScopePtr &scope);
	};
}

#endif
