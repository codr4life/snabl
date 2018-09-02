#include "snabl/env.hpp"
#include "snabl/error.hpp"
#include "snabl/scope.hpp"

namespace snabl {
	Scope::Scope(Env &env, const ScopePtr &parent): env(env), parent(parent) { }

	const Box *Scope::get_var(Sym id) const {
		const auto found(_vars.find(id));
		if (found != _vars.end()) { return &found->second; }
		return parent ? parent->get_var(id) : nullptr;
	}

	void Scope::put_var(Sym id, const optional<Box> &val) {
		auto found(env.is_safe() ? _vars.find(id) : _vars.end());
		
		if (found == _vars.end()) {
			if (val) {
				_vars.emplace(make_pair(id, *val)).first->second;
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
}
