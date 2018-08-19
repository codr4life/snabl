#include "snabl/macro.hpp"

namespace snabl {
	Macro::Macro(Lib &lib, const Sym &id, const Imp &imp):
		lib(lib), id(id), _imp(imp) { }

	void Macro::call(Forms::const_iterator &in,
									 const Forms::const_iterator &end,
									 AFuncPtr &func, AFimpPtr &fimp,
									 Bin &out) {
		_imp(in, end, func, fimp, out);
	}
}
