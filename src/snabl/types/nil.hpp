#ifndef SNABL_TYPE_NIL_HPP
#define SNABL_TYPE_NIL_HPP

#include "snabl/std.hpp"
#include "snabl/type.hpp"

namespace snabl {
	using Nil = nullptr_t;

	class NilType: public Type<Nil> {
	public:
		NilType(Lib &lib, Sym id);
		bool is_true(const Box &val) const override;
		void call(const Box &val, Pos pos, bool now) const override;
		void dump(const Box &val, ostream &out) const override;
	};
}

#endif
