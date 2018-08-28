#ifndef SNABL_INT_HPP
#define SNABL_INT_HPP

#include "snabl/std.hpp"
#include "snabl/type.hpp"

namespace snabl {
	using Int = long long;
	
	class IntType: public Type<Int> {
	public:
		IntType(Lib &lib, Sym id);
		void dump(const Box &value, ostream &out) const override;
	};
}

#endif
