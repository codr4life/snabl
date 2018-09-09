#include "snabl/box.hpp"
#include "snabl/types/str.hpp"

namespace snabl {
	StrType::StrType(Lib &lib, Sym id): Type<string>(lib, id) { }

	bool StrType::is_true(const Box &val) const { return !val.as<string>().empty(); }

	void StrType::dump(const Box &val, ostream &out) const {
		out << '\'' << val.as<string>() << '\'';
	}
}
