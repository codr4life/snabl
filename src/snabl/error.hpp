#ifndef SNABL_ERROR_HPP
#define SNABL_ERROR_HPP

#include <stdexcept>

#include "snabl/std.hpp"

namespace snabl {
	struct Pos;
	
	class Error: public runtime_error {
	public:
    Error(const string &msg);
	};

	class SyntaxError: public Error {
	public:
    SyntaxError(Pos pos, const string &msg);
	};
}

#endif
