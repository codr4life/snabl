#ifndef SNABL_CALL_HPP
#define SNABL_CALL_HPP

#include "snabl/types/lambda.hpp"
#include "snabl/op.hpp"
#include "snabl/ptrs.hpp"
#include "snabl/std.hpp"
#include "snabl/scope.hpp"

namespace snabl {
	class Call {
	public:
		const ScopePtr scope;
		const FimpPtr fimp;
		const optional<PC> return_pc;

		Call(const ScopePtr &scope, const FimpPtr &fimp, optional<PC> return_pc=nullopt);
		Call(const ScopePtr &scope, PC return_pc);
	};
}

#endif
