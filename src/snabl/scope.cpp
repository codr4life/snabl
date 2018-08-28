#include "snabl/env.hpp"
#include "snabl/error.hpp"
#include "snabl/scope.hpp"

namespace snabl {
	Scope::Scope(Env &env):
		env(env) { }

	Scope::~Scope() {
		for (auto &v: _put_vars) { env.unsafe_put_var(v.first, v.second); }
	}
	
	void Scope::put_var(Sym id, const Box &value) {
		auto found(_put_vars.find(id));
		if (found != _put_vars.end()) { throw Error("Duplicate var: " + id.name()); }
		auto prev(env.unsafe_put_var(id, value));
		if (prev) { _put_vars.emplace(std::make_pair(id, value)); }
	}
}
