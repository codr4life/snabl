#include "snabl/func.hpp"

namespace snabl {
	AFunc::AFunc(Lib &lib, const Sym &id, int nargs, int nrets):
		lib(lib), id(id), nargs(nargs), nrets(nrets) {
	}
}
