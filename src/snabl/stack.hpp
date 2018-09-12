#ifndef SNABL_STACK_HPP
#define SNABL_STACK_HPP

#include "snabl/box.hpp"

namespace snabl {
	using Stack = vector<Box>;
	using StackPtr = shared_ptr<Stack>;
	
	ostream &operator <<(ostream &out, const Stack &stack);
}

#endif
