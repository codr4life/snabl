#ifndef SNABL_TYPE_SYM_HPP
#define SNABL_TYPE_SYM_HPP

#include "snabl/std.hpp"
#include "snabl/type.hpp"

namespace snabl {
	class SymType: public Type<Sym> {
	public:
		SymType(Lib &lib, Sym id);
		void dump(const Box &val, ostream &out) const override;
	};
}

#endif
