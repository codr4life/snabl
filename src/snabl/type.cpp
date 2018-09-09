#include <ostream>

#include "snabl/env.hpp"
#include "snabl/lib.hpp"
#include "snabl/type.hpp"

namespace snabl {
	AType::AType(Lib &lib, Sym id): Def(id), lib(lib), tag(lib.env.next_type_tag()) { }
	
	void AType::call(const Box &val, Pos pos, bool now) const { lib.env.push(val); }
}
