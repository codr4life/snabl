#ifndef SNABL_TYPE_STR_HPP
#define SNABL_TYPE_STR_HPP

#include "snabl/std.hpp"
#include "snabl/type.hpp"

namespace snabl {
	using StrPtr = shared_ptr<string>;

	class StrType: public Type<StrPtr> {
	public:
		StrType(Lib &lib, Sym id);
		bool as_bool(const Box &val) const override;
		IterPtr iter(const Box &val) const;
		void dump(const Box &val, ostream &out) const override;
	};
}

#endif
