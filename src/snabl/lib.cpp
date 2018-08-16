#include "snabl/lib.hpp"

namespace snabl {
	Lib::Lib(const Sym &id): id(id) {
	}

	FuncPtr Lib::add_func(const Sym &id) {
		auto found(_funcs.find(id));
		if (found != _funcs.end()) { return found->second; }
		auto f(std::make_shared<Func>(id));
		_funcs.emplace(f->id, f);
		return f;
	}
}
