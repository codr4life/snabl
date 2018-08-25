#ifndef SNABL_BOOL_HPP
#define SNABL_BOOL_HPP

#include "snabl/type.hpp"

namespace snabl {
	class BoolType: public Type<bool> {
	public:
		BoolType(Lib &lib, const Sym &id);
		void dump(const Box &value, std::ostream &out) const override;
	};
}

#endif
