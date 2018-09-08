#ifndef SNABL_SCOPE_HPP
#define SNABL_SCOPE_HPP

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
		
		const Box *get_var(Sym id) const {
			const auto found(_vars.find(id));
			if (found != _vars.end()) { return &found->second; }
			return parent ? parent->get_var(id) : nullptr;
		}

		void put_var(Sym id, const optional<Box> &val) {
			auto found(_vars.find(id));
		
			if (found == _vars.end()) {
				if (val) {
					auto ok(_vars.emplace(make_pair(id, *val)));
				} else {
					throw Error("Missing var: " + id.name());				
				}
			} else {
				if (val) {
					throw Error("Duplicate var: " + id.name());
				} else {
					_vars.erase(found);
				}
			}
		}

		void clear_vars() { _vars.clear(); }
	private:
		map<Sym, Box> _vars;
	};
}

#endif
