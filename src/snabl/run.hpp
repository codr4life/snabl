#ifndef SNABL_RUN_HPP
#define SNABL_RUN_HPP

#include "snabl/call.hpp"
#include "snabl/stack.hpp"

namespace snabl {
	class Env;
	
	class RuntimeError: public Error {
	public:
		const Pos pos;
		const Stack stack;
		const vector<Call> calls;
		
		RuntimeError(Env &env, Pos _pos, const string &msg);
	};

	class UserError: public RuntimeError {
	public:
		const Box val;
		UserError(Env &env, Pos pos, const Box &_val);
	};

}

#endif
