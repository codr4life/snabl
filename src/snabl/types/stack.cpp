#include "snabl/box.hpp"
#include "snabl/types/stack.hpp"

namespace snabl {
	StackType::StackType(Lib &lib, Sym id): Type<StackPtr>(lib, id) { }

	bool StackType::is_true(const Box &val) const {
		return !val.as<StackPtr>()->empty();
	}

	void StackType::dump(const Box &val, ostream &out) const {
		out << *val.as<StackPtr>();
	}
}
