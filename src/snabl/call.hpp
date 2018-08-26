#ifndef SNABL_CALL_HPP
#define SNABL_CALL_HPP

#include "snabl/std/optional.hpp"
#include "snabl/scope.hpp"
#include "snabl/target.hpp"

namespace snabl {
	class Call {
	public:
		const TargetPtr target;
		const ScopePtr scope;
		const std::optional<size_t> return_pc;
		
		Call(const TargetPtr &target,
				 const ScopePtr &scope,
				 std::optional<std::size_t> return_pc);
	};
}

#endif
