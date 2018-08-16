#ifndef SNABL_INT_HPP
#define SNABL_INT_HPP

#include "snabl/type.hpp"

namespace snabl {
	using Int = long;
	
	class IntType: public Type<Int> {
	public:
		IntType(const Sym &id);
	};
}

#endif
