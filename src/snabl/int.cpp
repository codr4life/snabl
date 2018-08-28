#include <ostream>

#include "snabl/box.hpp"
#include "snabl/int.hpp"

namespace snabl {
	IntType::IntType(Lib &lib, Sym id): Type<Int>(lib, id) { }

	void IntType::dump(const Box &value, ostream &out) const {
		out << value.as<Int>();
	}
}
