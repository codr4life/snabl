#ifndef SNABL_TYPE_STR_HPP
#define SNABL_TYPE_STR_HPP

#include "snabl/std.hpp"
#include "snabl/type.hpp"

namespace snabl {
	class StrType: public Type<string> {
	public:
		StrType(Lib &lib, Sym id);
		bool is_true(const Box &val) const override;
		void dump(const Box &val, ostream &out) const override;
	};
}

#endif
