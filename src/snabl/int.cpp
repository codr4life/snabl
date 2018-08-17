#include <ostream>

#include "snabl/box.hpp"
#include "snabl/int.hpp"

namespace snabl {
	IntType::IntType(const Sym &id): Type<Int>(id) { }

	void IntType::dump(const Box &value, std::ostream &out) {
		out << value.as<Int>();
	}
}
