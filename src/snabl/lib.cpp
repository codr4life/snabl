#include "snabl/lib.hpp"

namespace snabl {
	Lib::Lib(Env &env, const Sym &id): env(env), id(id) { }

	AFuncPtr Lib::get_func(const Sym &id) {
		auto found(_funcs.find(id));
		if (found == _funcs.end()) { throw Error("Unknown func"); }
		return found->second;
	}
}
