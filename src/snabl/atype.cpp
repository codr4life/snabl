#include "snabl/env.hpp"
#include "snabl/lib.hpp"
#include "snabl/atype.hpp"

namespace snabl {
	AType::AType(Lib &lib, Sym id, Int size):
		Def(id), lib(lib), size(size), tag(lib.env.next_type_tag()) { }
	
	void AType::call(const Box &val, Pos pos) const { lib.env.push(val); }

	optional<Box> AType::peek(const Box &source) const {
		throw Error(fmt("Invalid source: %0", {source}));
		return nullopt;
	}
}
