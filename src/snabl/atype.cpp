#include "snabl/env.hpp"
#include "snabl/lib.hpp"
#include "snabl/atype.hpp"

namespace snabl {
	AType::AType(Lib &lib, Sym id, size_t size):
		Def(id), lib(lib), size(size), tag(lib.env.next_type_tag()) { }
	
	void AType::call(const Box &val, Pos pos, bool now) const { lib.env.push(val); }
}
