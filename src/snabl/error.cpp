#include "snabl/fmt.hpp"
#include "snabl/pos.hpp"
#include "snabl/error.hpp"

namespace snabl {
	Error::Error(const std::string &msg): std::runtime_error(msg) { }

	SyntaxError::SyntaxError(Pos pos, const std::string &msg):
		Error(fmt("Syntax error in row %0, col %1: %2", {pos.row, pos.col, msg})) { }
}
