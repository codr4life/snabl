#ifndef SNABL_RUN_HPP
#define SNABL_RUN_HPP

#include "snabl/call.hpp"
#include "snabl/stack.hpp"

namespace snabl {
	class Env;
	
	class RuntimeError: public Error {
	public:
		RuntimeError(Env &env, Pos _pos, const string &msg);
		const char *what() const noexcept override;
	private:
		string _what;
	};

	class UserError: public RuntimeError {
	public:
		const Box val;
		UserError(Env &env, Pos pos, const Box &_val);
	};

}

#endif
