#include <ostream>

#include "snabl/box.hpp"
#include "snabl/pos.hpp"
#include "snabl/types/nil.hpp"

namespace snabl {
	NilType::NilType(Lib &lib, Sym id): Type<Nil>(lib, id) { }

	bool NilType::is_true(const Box &val) const { return false; }

	void NilType::call(const Box &val, Pos pos, bool now) const { }

	void NilType::dump(const Box &val, ostream &out) const {
		out << "nil";
	}
}
