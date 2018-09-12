#ifndef SNABL_SCOPE_HPP
#define SNABL_SCOPE_HPP

#include "snabl/alloc.hpp"
#include "snabl/ptrs.hpp"
#include "snabl/stack.hpp"
#include "snabl/std.hpp"
#include "snabl/sym.hpp"

namespace snabl {
	class Env;

	class Scope {
	public:
		Env &env;
		const ScopePtr parent;
		
		Scope(Env &env, const ScopePtr &parent): env(env), parent(parent) { }
		Scope(const Scope &) = delete;
		const Scope &operator=(const Scope &) = delete;
		
		const Box *get(Sym id) const {
			const auto found(_vars.find(id));
			if (found != _vars.end()) { return &found->second; }
			return parent ? parent->get(id) : nullptr;
		}

		void let(Sym id, const Box &val) {
			const auto ok(_vars.emplace(make_pair(id, val)));
			if (!ok.second) { throw Error("Duplicate var: " + id.name()); }
		}

		void clear_vars() { _vars.clear(); }
	private:
		map<Sym, Box, less<Sym>, Alloc<pair<Sym, Box>>> _vars;
	};
}

#endif
