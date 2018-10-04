#include "snabl/macro.hpp"

namespace snabl {
	Macro::Macro(Lib &lib, Sym id, const Imp &imp): Def(id), lib(lib), _imp(imp) { }

	void Macro::call(Forms::const_iterator &in,
									 Forms::const_iterator end,
									 Env &env) { _imp(in, end, env); }
}
