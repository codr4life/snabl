#include "snabl/box.hpp"
#include "snabl/types/meta.hpp"

namespace snabl {
	MetaType::MetaType(Lib &lib, Sym id): Type<ATypePtr>(lib, id) { }

	void MetaType::dump(const Box &val, ostream &out) const {
		out << val.as<ATypePtr>()->id;
	}
}
