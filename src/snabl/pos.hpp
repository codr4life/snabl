#ifndef SNABL_POS_HPP
#define SNABL_POS_HPP

#include <cstddef>

namespace snabl {
	struct Pos {
		Pos(std::size_t row, std::size_t col): row(row), col(col) { } 
		std::size_t row, col;
	};
}

#endif
