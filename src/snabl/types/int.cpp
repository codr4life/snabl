#include "snabl/box.hpp"
#include "snabl/env.hpp"
#include "snabl/types/int.hpp"

namespace snabl {
	IntType::IntType(Lib &lib, Sym id): Type<Int>(lib, id) { }

	bool IntType::is_true(const Box &val) const { return val.as<Int>(); }

	void IntType::dump(const Box &val, ostream &out) const {
		out << val.as<Int>();
	}

	IterPtr IntType::iter(const Box &val) const {
		Int i(0);
		const Int max(val.as<Int>());
		
		return make_shared<Iter>([i, max](Env &env) mutable {
				return (i < max) ? make_optional<Box>(env.int_type, i++) : nullopt;
			});
	}
}
