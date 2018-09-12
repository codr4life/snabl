#include "snabl/lib.hpp"

namespace snabl {
	Lib::Lib(Env &env, Sym id): Def(id), env(env) { }

	Lib::~Lib() {
		for (auto &f: _funcs) { f.second->clear(); }
	}

	const MacroPtr &Lib::add_macro(Sym id, const Macro::Imp &imp) {
		auto found(_macros.find(id));
		if (found != _macros.end()) { _macros.erase(found); }
		return _macros.emplace(id, make_shared<Macro>(*this, id, imp)).first->second;
	}
	
	const FuncPtr &Lib::add_func(Sym id, size_t nargs) {
		auto found(_funcs.find(id));
		
		if (found != _funcs.end()) {
			auto &f(found->second);
			if (f->nargs != nargs) { throw Error("Args mismatch"); }
			return f;
		}
		
		return _funcs.emplace(id, make_shared<Func>(*this, id, nargs)).first->second;
	}

	
	const MacroPtr *Lib::get_macro(Sym id) {
		auto found(_macros.find(id));
		return (found == _macros.end()) ? nullptr : &found->second;
	}

	const ATypePtr *Lib::get_type(Sym id) {
		auto found(_types.find(id));
		return (found == _types.end()) ? nullptr : &found->second;
	}
	
	const FuncPtr *Lib::get_func(Sym id) {
		auto found(_funcs.find(id));
		return (found == _funcs.end()) ? nullptr : &found->second;
	}
}
