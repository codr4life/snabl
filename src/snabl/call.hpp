#ifndef SNABL_CALL_HPP
#define SNABL_CALL_HPP

#include "snabl/fimp.hpp"
#include "snabl/scope.hpp"

namespace snabl {
	class Call {
	public:
		const AFimpPtr fimp;
		const ScopePtr scope;
		
		Call(const AFimpPtr &fimp, const ScopePtr &scope);
	};
}

#endif
