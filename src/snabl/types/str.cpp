#include "snabl/box.hpp"
#include "snabl/env.hpp"
#include "snabl/iter.hpp"
#include "snabl/types/int.hpp"
#include "snabl/types/str.hpp"

namespace snabl {
	StrType::StrType(Lib &lib, Sym id): Type<StrPtr>(lib, id) { }

	bool StrType::as_bool(const Box &val) const { return !val.as<StrPtr>()->empty(); }

	bool StrType::eqval(const Box &lhs, const Box &rhs) const {
		return *lhs.as<StrPtr>() == *rhs.as<StrPtr>();
	}

	void StrType::dump(const Box &val, ostream &out) const {
		out << "''" << *val.as<StrPtr>() << "''";
	}

	IterPtr StrType::iter(const Box &val) const {
		const StrPtr s(val.as<StrPtr>());
		auto i(s->begin());
		
		return make_shared<Iter>([s, i](Env &env) mutable {
				return (i == s->end())
					? nullopt
					: make_optional<Box>(env.int_type, Int(*i++));
			});
	}
}
