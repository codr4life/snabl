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
		Pos pos;
		State state;
		
		Fimp *const fimp;
		LambdaPtr lambda;
		optional<PC> return_pc;

		Call(Scope &scope, Pos pos, Fimp &fimp, optional<PC> return_pc=nullopt):
			scope(scope), pos(pos), state(scope.env), fimp(&fimp), lambda(nullptr),
			return_pc(return_pc) { }
	
		Call(Scope &scope, Pos pos, const LambdaPtr &lambda, PC return_pc):
			scope(scope), pos(pos), state(scope.env), fimp(nullptr), lambda(lambda),
			return_pc(return_pc) { }

		void recall() const;
	};
}

#endif
