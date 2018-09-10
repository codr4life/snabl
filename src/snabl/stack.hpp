#ifndef SNABL_STACK_HPP
#define SNABL_STACK_HPP

#include "snabl/alloc.hpp"
#include "snabl/box.hpp"

namespace snabl {
	using Stack = deque<Box, Alloc<Box>>;

	ostream &operator <<(ostream &out, const Stack &stack);
}

#endif
