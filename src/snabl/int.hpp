#ifndef SNABL_INT_HPP
#define SNABL_INT_HPP

#include "snabl/type.hpp"

namespace snabl {
	using Int = long long;
	
	class IntType: public Type<Int> {
	public:
		IntType(Lib &lib, const Sym &id);
		void dump(const Box &value, std::ostream &out) const override;
	};
}

#endif
