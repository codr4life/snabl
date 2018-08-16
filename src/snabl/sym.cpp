#include "snabl/sym.hpp"

namespace snabl {
	SymImp::SymImp(const std::string &name):
		_name(name), _hash(std::hash<std::string>{}(name)) { }

	Sym::Sym(const SymImp *imp): _imp(imp) { }

	bool operator==(const Sym &x, const Sym &y) { return x._imp == y._imp; }

	bool operator!=(const Sym &x, const Sym &y) { return x._imp != y._imp; }
}
