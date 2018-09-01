#include "snabl/lib.hpp"

namespace snabl {
	Lib::Lib(Env &env, Sym id): env(env), id(id) { }

	Lib::~Lib() {
		for (auto &f: _funcs) { f.second->clear(); }
	}

	MacroPtr Lib::add_macro(Sym id, const Macro::Imp &imp) {
		auto found(_macros.find(id));
		if (found != _macros.end()) { _macros.erase(found); }
		return _macros.emplace(id, MacroPtr(MacroPtr::Make(), *this, id, imp))
			.first->second;
	}
	
	FuncPtr Lib::add_func(Sym id, size_t nargs, size_t nrets) {
		auto found(_funcs.find(id));
		
		if (found != _funcs.end()) {
			auto f(found->second);

			if (f->nargs != nargs) { throw Error("Args mismatch"); }
			if (f->nrets != nrets) { throw Error("Rets mismatch"); }
			return f;
		}
		
		return _funcs.emplace(id, FuncPtr(FuncPtr::Make(), *this, id, nargs, nrets))
			.first->second;
	}

	
	MacroPtr Lib::get_macro(Sym id) {
		auto found(_macros.find(id));
		return (found == _macros.end()) ? nullptr : found->second;
	}

	ATypePtr Lib::get_type(Sym id) {
		auto found(_types.find(id));
		return (found == _types.end()) ? nullptr : found->second;
	}
	
	FuncPtr Lib::get_func(Sym id) {
		auto found(_funcs.find(id));
		return (found == _funcs.end()) ? nullptr : found->second;
	}
}
