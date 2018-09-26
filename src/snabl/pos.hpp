#ifndef SNABL_POS_HPP
#define SNABL_POS_HPP

#include "snabl/std.hpp"
#include "snabl/types.hpp"

namespace snabl {
	struct Pos {
		Pos(Int row, Int col): row(row), col(col) { } 
		Int row, col;
	};
}

#endif
