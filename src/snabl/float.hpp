#ifndef SNABL_FLOAT_HPP
#define SNABL_FLOAT_HPP

#include "snabl/type.hpp"

namespace snabl {
	using Float = long double;
	
	class FloatType: public Type<Float> {
	public:
		FloatType(Lib &lib, Sym id);
		void dump(const Box &value, std::ostream &out) const override;
	};
}

#endif
