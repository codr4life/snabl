#include "snabl/env.hpp"
#include "snabl/scope.hpp"

namespace snabl {
	Scope::Scope(Env &env, const ScopePtr &parent):
		env(env), parent(parent), stack_begin(env.stack_begin()) {
	}

	Box const* Scope::get(const Sym &key) {
		auto found = _vars.find(key);
		return (found == _vars.end()) ? nullptr : &found->second;
	}
}
