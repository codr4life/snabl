#include "snabl/error.hpp"

namespace snabl {
	Error::Error(const std::string &msg): std::runtime_error(msg) { }

	SyntaxError::SyntaxError(const Pos &pos, const std::string &msg): Error(msg) { }
}
