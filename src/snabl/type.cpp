#include <ostream>

#include "snabl/env.hpp"
#include "snabl/lib.hpp"
#include "snabl/type.hpp"

namespace snabl {
	AType::AType(Lib &lib, Sym id): lib(lib), tag(lib.env.next_type_tag()), id(id) { }
	
	void AType::call(const Box &val, bool now) const { lib.env.push(val); }
}
