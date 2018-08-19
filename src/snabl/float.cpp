#include <ostream>

#include "snabl/box.hpp"
#include "snabl/float.hpp"

namespace snabl {
	FloatType::FloatType(Lib &lib, const Sym &id): Type<Float>(lib, id) { }

	void FloatType::dump(const Box &value, std::ostream &out) {
		out << value.as<Float>();
	}
}
