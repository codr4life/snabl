#ifndef SNABL_TYPE_STACK_HPP
#define SNABL_TYPE_STACK_HPP

#include "snabl/stack.hpp"
#include "snabl/std.hpp"
#include "snabl/type.hpp"

namespace snabl {
	class StackType: public Type<StackPtr> {
	public:
		StackType(Lib &lib, Sym id);
		bool as_bool(const Box &val) const override;
		IterPtr iter(const Box &val) const;
		void dump(const Box &val, ostream &out) const override;
	};
}

#endif
