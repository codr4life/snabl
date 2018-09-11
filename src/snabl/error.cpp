#include "snabl/fmt.hpp"
#include "snabl/pos.hpp"
#include "snabl/error.hpp"

namespace snabl {
	Error::Error(const string &msg): runtime_error(msg) { }

	CompileError::CompileError(Pos pos, const string &msg):
		Error(fmt("Compile error in row %0, col %1: %2", {pos.row, pos.col, msg})) { }

	CompileError::CompileError(const string &msg): Error(msg) { }

	SyntaxError::SyntaxError(Pos pos, const string &msg):
		CompileError(fmt("Syntax error in row %0, col %1: %2",
										 {pos.row, pos.col, msg})) { }
}
