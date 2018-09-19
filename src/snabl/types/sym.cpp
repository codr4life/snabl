#include <ostream>

#include "snabl/box.hpp"
#include "snabl/types/sym.hpp"

namespace snabl {
	SymType::SymType(Lib &lib, Sym id): Type<Sym>(lib, id) { }

	void SymType::dump(const Box &val, ostream &out) const {
		out << '\'' << val.as<Sym>();
	}
}
