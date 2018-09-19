#ifndef SNABL_CALL_HPP
#define SNABL_CALL_HPP

#include "snabl/pos.hpp"
#include "snabl/ptrs.hpp"
#include "snabl/state.hpp"
#include "snabl/std.hpp"

namespace snabl {	
	class Call {
	public:
		const TargetPtr parent;
		const Pos pos;
		const optional<const State> state;
		const PC return_pc;

		Call(const TargetPtr &parent,
				 Pos pos,
				 const optional<State> &state=nullopt,
				 PC return_pc=nullptr):
			parent(parent), pos(pos), state(state), return_pc(return_pc) { }
	};
}

#endif
