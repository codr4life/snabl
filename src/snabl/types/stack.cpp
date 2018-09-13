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
		size_t i(0);
		
		return make_shared<Iter>([s, i](Env &env) mutable {
				return (i < s->size())
					? make_optional<Box>((*s)[i])
					: nullopt;
			});
	}

	void StackType::dump(const Box &val, ostream &out) const {
		out << *val.as<StackPtr>();
	}
}
