#include "snabl/box.hpp"
#include "snabl/env.hpp"
#include "snabl/types/char.hpp"

namespace snabl {
	CharType::CharType(Lib &lib, Sym id): Type<Char>(lib, id) { }

	bool CharType::as_bool(const Box &val) const { return val.as<Char>(); }

	void CharType::dump(const Box &val, ostream &out) const {
		Env &env(val.type()->lib.env);
		const auto c(val.as<Char>());
		const auto sc(env.find_char_special(c));
		
		if (sc) {
			out << '~' << *sc;
		} else {
			out << '#' << c;
		}
	}
}
