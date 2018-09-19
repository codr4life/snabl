#ifndef SNABL_CALL_HPP
#define SNABL_CALL_HPP

#include "snabl/fimp.hpp"
#include "snabl/lambda.hpp"
#include "snabl/op.hpp"
#include "snabl/ptrs.hpp"
#include "snabl/state.hpp"
#include "snabl/std.hpp"
#include "snabl/scope.hpp"

namespace snabl {	
	class Call {
	public:
		const Pos pos;
		
		const TargetPtr target;
		const PC return_pc;

		Call(Env &env, Pos pos, const FimpPtr &fimp, PC return_pc=nullptr):
			pos(pos), target(fimp), return_pc(return_pc) { }
	
		Call(Env &env, Pos pos, const LambdaPtr &lambda, PC return_pc):
			pos(pos), target(lambda), return_pc(return_pc) { }
	};
}

#endif
