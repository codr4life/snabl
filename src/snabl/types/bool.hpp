#ifndef SNABL_TYPE_BOOL_HPP
#define SNABL_TYPE_BOOL_HPP

#include "snabl/std.hpp"
#include "snabl/type.hpp"

namespace snabl {
	class BoolType: public Type<bool> {
	public:
		BoolType(Lib &lib, Sym id);
		bool as_bool(const Box &val) const override;
		void dump(const Box &val, ostream &out) const override;
	};
}

#endif
