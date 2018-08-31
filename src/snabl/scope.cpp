#include "snabl/env.hpp"
#include "snabl/error.hpp"
#include "snabl/scope.hpp"

namespace snabl {
	Scope::Scope(Env &env, ScopePtr parent): env(env), parent(parent) { }

	optional<Box> Scope::get_var(Sym id) const {
		const auto found(_vars.find(id));
		if (found != _vars.end()) { return make_optional(found->second); }
		return parent ? parent->get_var(id) : nullopt;
	}

	void Scope::put_var(Sym id, const optional<Box> &val) {
		auto found(_vars.find(id));

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
