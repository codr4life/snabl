#include "snabl/env.hpp"
#include "snabl/error.hpp"
#include "snabl/scope.hpp"

namespace snabl {
	Scope::Scope(Env &env): env(env) { }

	optional<Box> Scope::get_var(Sym id) const {
		const auto found(_vars.find(id));
		return (found == _vars.end()) ? nullopt : make_optional(found->second);
	}

	void Scope::put_var(Sym id, const optional<Box> &value) {
		auto found(_vars.find(id));

		if (found == _vars.end()) {
			if (value) {
				_vars.emplace(make_pair(id, *value)).first->second;
			} else {
				throw Error("Missing var: " + id.name());				
			}
		} else {
			if (value) {
				throw Error("Duplicate var: " + id.name());
			} else {
				_vars.erase(found);
			}
		}
	}
}
