#include <ostream>

#include "snabl/box.hpp"
#include "snabl/int.hpp"

namespace snabl {
	IntType::IntType(Lib &lib, const Sym &id): Type<Int>(lib, id) { }

	void IntType::dump(const Box &value, std::ostream &out) {
		out << value.as<Int>();
	}
}
