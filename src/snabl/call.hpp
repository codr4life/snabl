#ifndef SNABL_CALL_HPP
#define SNABL_CALL_HPP

#include "snabl/fimp.hpp"
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
		const Pos pos;
		const State state;
		
		const TargetPtr target;
		const optional<PC> return_pc;

		Call(Scope &scope, Pos pos, const FimpPtr &fimp, optional<PC> return_pc=nullopt):
			scope(scope),
			pos(pos),
			state(scope.env),
			target(fimp),
			return_pc(return_pc) { }
	
		Call(Scope &scope, Pos pos, const LambdaPtr &lambda, PC return_pc):
			scope(scope),
			pos(pos),
			state(scope.env),
			target(lambda),
			return_pc(return_pc) { }

		void recall() const;
	};
}

#endif
