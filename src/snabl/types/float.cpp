#include <ostream>

#include "snabl/box.hpp"
#include "snabl/types/float.hpp"

namespace snabl {
	FloatType::FloatType(Lib &lib, Sym id): Type<Float>(lib, id) { }

	void FloatType::dump(const Box &val, ostream &out) const {
		out << val.as<Float>();
	}
}
