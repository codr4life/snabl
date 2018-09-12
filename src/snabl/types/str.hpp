#ifndef SNABL_TYPE_STR_HPP
#define SNABL_TYPE_STR_HPP

#include "snabl/std.hpp"
#include "snabl/type.hpp"

namespace snabl {
	using StrPtr = shared_ptr<string>;

	class StrType: public Type<StrPtr> {
	public:
		StrType(Lib &lib, Sym id);
		bool is_true(const Box &val) const override;
		void dump(const Box &val, ostream &out) const override;
	};
}

#endif
