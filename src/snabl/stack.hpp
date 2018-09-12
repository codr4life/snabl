#ifndef SNABL_STACK_HPP
#define SNABL_STACK_HPP

#include "snabl/box.hpp"

namespace snabl {
	using Stack = vector<Box>;

	ostream &operator <<(ostream &out, const Stack &stack);
}

#endif
