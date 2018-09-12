#ifndef SNABL_TYPE_STACK_HPP
#define SNABL_TYPE_STACK_HPP

#include "snabl/stack.hpp"
#include "snabl/std.hpp"
#include "snabl/type.hpp"

namespace snabl {
	class StackType: public Type<StackPtr> {
	public:
		StackType(Lib &lib, Sym id);
		bool is_true(const Box &val) const override;
		void dump(const Box &val, ostream &out) const override;
	};
}

#endif
