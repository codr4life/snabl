#ifndef SNABL_POS_HPP
#define SNABL_POS_HPP

#include "snabl/std.hpp"

namespace snabl {
	struct Pos {
		Pos(size_t row, size_t col): row(row), col(col) { } 
		size_t row, col;
	};
}

#endif
