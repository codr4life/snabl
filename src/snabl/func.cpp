#include "snabl/func.hpp"

namespace snabl {
	AFunc::AFunc(Lib &lib, Sym id, int nargs, int nrets):
		lib(lib), id(id), nargs(nargs), nrets(nrets) {
	}
}
