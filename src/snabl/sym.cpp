#include "snabl/sym.hpp"

namespace snabl {
	Sym::Sym(const std::string &name): _name(name) {
	}

	bool operator==(const Sym &x, const Sym &y) {
		return &x == &y;
	}

	bool operator!=(const Sym &x, const Sym &y) {
		return &x != &y;
	}
}
