#ifndef SNABL_BOOL_HPP
#define SNABL_BOOL_HPP

#include "snabl/std.hpp"
#include "snabl/type.hpp"

namespace snabl {
	class BoolType: public Type<bool> {
	public:
		BoolType(Lib &lib, Sym id);
		void dump(const Box &val, ostream &out) const override;
	};
}

#endif
