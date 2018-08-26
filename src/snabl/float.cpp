#include <ostream>

#include "snabl/box.hpp"
#include "snabl/float.hpp"

namespace snabl {
	FloatType::FloatType(Lib &lib, Sym id): Type<Float>(lib, id) { }

	void FloatType::dump(const Box &value, std::ostream &out) const {
		out << value.as<Float>();
	}
}
