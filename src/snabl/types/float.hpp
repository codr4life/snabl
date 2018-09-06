#ifndef SNABL_TYPE_FLOAT_HPP
#define SNABL_TYPE_FLOAT_HPP

#include "snabl/std.hpp"
#include "snabl/type.hpp"

namespace snabl {
	using Float = long double;
	
	class FloatType: public Type<Float> {
	public:
		FloatType(Lib &lib, Sym id);
		void dump(const Box &val, ostream &out) const override;
	};
}

#endif
