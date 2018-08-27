#ifndef SNABL_CALL_HPP
#define SNABL_CALL_HPP

#include "snabl/stdx/optional.hpp"
#include "snabl/scope.hpp"
#include "snabl/target.hpp"

namespace snabl {
	class Call {
	public:
		const TargetPtr target;
		const ScopePtr scope;
		const stdx::optional<size_t> return_pc;
		
		Call(const TargetPtr &target,
				 const ScopePtr &scope,
				 stdx::optional<std::size_t> return_pc);
	};
}

#endif
