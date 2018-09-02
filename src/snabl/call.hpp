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
		Scope &scope;
		Fimp *const fimp;
		Lambda *const lambda;
		const optional<PC> return_pc;

		Call(Scope &scope, Fimp &fimp, optional<PC> return_pc=nullopt);
		Call(Scope &scope, Lambda &lambda, PC return_pc);
	};
}

#endif
