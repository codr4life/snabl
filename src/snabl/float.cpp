#include <ostream>

#include "snabl/box.hpp"
#include "snabl/float.hpp"

namespace snabl {
	FloatType::FloatType(const Sym &id): Type<Float>(id) { }

	void FloatType::dump(const Box &value, std::ostream &out) {
		out << value.as<Float>();
	}
}
