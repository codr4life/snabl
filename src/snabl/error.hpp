#ifndef SNABL_ERROR_HPP
#define SNABL_ERROR_HPP

#include <stdexcept>
#include <string>

namespace snabl {
	struct Pos;
	
	class Error: public std::runtime_error {
	public:
    Error(const std::string &msg);
	};

	class SyntaxError: public Error {
	public:
    SyntaxError(Pos pos, const std::string &msg);
	};
}

#endif
