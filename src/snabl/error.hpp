#ifndef SNABL_ERROR_HPP
#define SNABL_ERROR_HPP

#include <stdexcept>

#include "snabl/pos.hpp"
#include "snabl/std.hpp"

namespace snabl {
	class Error: public runtime_error {
	public:
    Error(const string &msg);
	protected:
		Error(): runtime_error("") { }
	};

	class CompileError: public Error {
	public:
    CompileError(Pos pos, const string &msg);
	protected:
    CompileError(const string &msg);
	};

	class SyntaxError: public CompileError {
	public:
    SyntaxError(Pos pos, const string &msg);
	};
}

#endif
