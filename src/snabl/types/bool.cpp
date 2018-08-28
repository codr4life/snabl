#include <ostream>

#include "snabl/box.hpp"
#include "snabl/types/bool.hpp"

namespace snabl {
	BoolType::BoolType(Lib &lib, Sym id): Type<bool>(lib, id) { }

	void BoolType::dump(const Box &value, ostream &out) const {
		out << (value.as<bool>() ? 't' : 'f');
	}
}
