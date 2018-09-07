#ifndef SNABL_CALL_HPP
#define SNABL_CALL_HPP

#include "snabl/op.hpp"
#include "snabl/ptrs.hpp"
#include "snabl/state.hpp"
#include "snabl/std.hpp"
#include "snabl/scope.hpp"
#include "snabl/types/lambda.hpp"

namespace snabl {
	class Call {
	public:
		Scope &scope;
		const State state;

		Fimp *const fimp;
		const LambdaPtr lambda;
		const optional<PC> return_pc;
		
		Call(Scope &scope, Fimp &fimp, optional<PC> return_pc=nullopt):
			scope(scope), state(scope.env), fimp(&fimp), lambda(nullptr),
			return_pc(return_pc) { }
	
		Call(Scope &scope, const LambdaPtr &lambda, PC return_pc):
			scope(scope), state(scope.env), fimp(nullptr), lambda(lambda),
			return_pc(return_pc) { }

		void recall() const;
	};
}

#endif
