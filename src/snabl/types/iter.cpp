#include "snabl/box.hpp"
#include "snabl/env.hpp"
#include "snabl/lib.hpp"
#include "snabl/types/iter.hpp"

namespace snabl {
	IterType::IterType(Lib &lib, Sym id): Type<IterPtr>(lib, id) { }

	bool IterType::is_true(const Box &val) const {
		return !val.as<IterPtr>()->is_done();
	}

	void IterType::call(const Box &val, Pos pos, bool now) const {
		Env &env(val.type()->lib.env);
		const auto v(val.as<IterPtr>()->call(env));
		if (v) { env.push(*v); } else { env.push(env.nil_type); }
	}

	IterPtr IterType::iter(const Box &val) const {
		return val.as<IterPtr>();
	}

	void IterType::dump(const Box &val, ostream &out) const {
		out << "Iter(" << val.as<IterPtr>().get() << ')';
	}
}
