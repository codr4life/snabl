#include "snabl/box.hpp"
#include "snabl/iter.hpp"
#include "snabl/types/stack.hpp"

namespace snabl {
	class Env;
	
	StackType::StackType(Lib &lib, Sym id): Type<StackPtr>(lib, id) { }

	bool StackType::as_bool(const Box &val) const {
		return !val.as<StackPtr>()->empty();
	}

	IterPtr StackType::iter(const Box &val) const {
		const StackPtr s(val.as<StackPtr>());
		auto i(s->begin());
		
		return make_shared<Iter>([s, i](Env &env) mutable {
				return (i == s->end())
					? nullopt
					: make_optional<Box>(*i++);
			});
	}

	void StackType::dump(const Box &val, ostream &out) const {
		out << *val.as<StackPtr>();
	}
}
