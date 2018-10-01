#ifndef SNABL_TYPE_ITER_HPP
#define SNABL_TYPE_ITER_HPP

#include "snabl/iter.hpp"
#include "snabl/std.hpp"
#include "snabl/type.hpp"

namespace snabl {	
	class IterType: public Type<IterPtr> {
	public:
		IterType(Lib &lib, Sym id);
		bool as_bool(const Box &val) const override;
		void call(const Box &val, Pos pos) const override;
		IterPtr iter(const Box &val) const override;
		void dump(const Box &val, ostream &out) const override;
	};
}

#endif
