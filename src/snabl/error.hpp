#ifndef SNABL_ERROR_HPP
#define SNABL_ERROR_HPP

#include <stdexcept>

#include "snabl/pos.hpp"
#include "snabl/std.hpp"

namespace snabl {
	class Error: public runtime_error {
	public:
    Error(const string &msg);
	};

	class SyntaxError: public Error {
	public:
		const Pos pos;
		
    SyntaxError(Pos _pos, const string &msg);
	};
}

#endif
