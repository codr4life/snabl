#include "snabl/sym.hpp"

namespace snabl {
	SymImp::SymImp(const std::string &name):
		name(name), hash(std::hash<std::string>{}(name)) { }

	Sym::Sym(const SymImp *imp): _imp(imp) { }
	
	const std::string &Sym::name() const { return _imp->name; }

	bool operator==(const Sym &x, const Sym &y) { return x._imp == y._imp; }

	bool operator!=(const Sym &x, const Sym &y) { return x._imp != y._imp; }
}
