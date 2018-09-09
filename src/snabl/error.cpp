#include "snabl/fmt.hpp"
#include "snabl/pos.hpp"
#include "snabl/error.hpp"

namespace snabl {
	Error::Error(const string &msg): runtime_error(msg) { }

	SyntaxError::SyntaxError(Pos _pos, const string &msg):
		Error(fmt("Syntax error in row %0, col %1: %2", {pos.row, pos.col, msg})),
		pos(_pos) { }
}
