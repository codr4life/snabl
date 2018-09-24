#ifndef SNABL_TASK_HPP
#define SNABL_TASK_HPP

#include "snabl/op.hpp"
#include "snabl/state.hpp"

namespace snabl {
	class Env;
	
	struct Task {
		Task(): _pc(nullptr) { }
	private:
		optional<State> _state;
		Ops _ops;
		PC _pc;

		friend Env;
	};
}

#endif
