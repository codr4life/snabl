#include "snabl/sym.hpp"

namespace snabl {
	bool operator ==(Sym x, Sym y) { return x._imp == y._imp; }

	bool operator !=(Sym x, Sym y) { return x._imp != y._imp; }

	bool operator <(Sym x, Sym y) { return x._imp < y._imp; }

	string fmt_arg(const Sym &x) { return x.name(); }
}
