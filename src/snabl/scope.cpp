#include "snabl/env.hpp"
#include "snabl/error.hpp"
#include "snabl/scope.hpp"

namespace snabl {
	Scope::Scope(Env &env): env(env) { }

	Scope::~Scope() {
		for (auto &v: _put_vars) {
			env.unsafe_put_var(v.first, v.second);
		}
	}
	
	void Scope::put_var(Sym id, const optional<Box> &value) {
		auto found(_put_vars.find(id));
		bool erased(false);
		
		if (found != _put_vars.end()) {
			if (value) {
				throw Error("Duplicate var: " + id.name());
			} else {
				_put_vars.erase(found);
				erased = true;
			}
		}
		
		auto prev(env.unsafe_put_var(id, value));
		if (prev && !erased) {
			_put_vars.emplace(make_pair(id, *prev));
		}
	}
}
