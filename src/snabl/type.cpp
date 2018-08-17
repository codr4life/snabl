#include <ostream>
#include "snabl/type.hpp"

namespace snabl {
	AType::AType(const Sym &id): id(id) { }

	void AType::dump(const Box &value, std::ostream &out) {
		out << id.name() << "(?)";
	}
}
