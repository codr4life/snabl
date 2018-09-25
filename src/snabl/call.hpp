#ifndef SNABL_CALL_HPP
#define SNABL_CALL_HPP

#include "snabl/pos.hpp"
#include "snabl/ptrs.hpp"
#include "snabl/state.hpp"
#include "snabl/std.hpp"

namespace snabl {	
	struct Call {
		const TargetPtr target;
		const Pos pos;
		const State state;
		const PC return_pc;

		Call(Env &env,
				 const TargetPtr &target,
				 Pos pos,
				 PC return_pc=nullptr):
			target(target), pos(pos), state(env), return_pc(return_pc) { }
	};
}

#endif
