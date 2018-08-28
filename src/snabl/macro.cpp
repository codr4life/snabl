#include "snabl/macro.hpp"

namespace snabl {
	Macro::Macro(Lib &lib, Sym id, const Imp &imp):
		lib(lib), id(id), _imp(imp) { }

	void Macro::call(Forms::const_iterator &in,
									 Forms::const_iterator end,
									 FuncPtr &func, FimpPtr &fimp,
									 Env &env) {
		_imp(in, end, func, fimp, env);
	}
}
