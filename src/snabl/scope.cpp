#include "snabl/env.hpp"
#include "snabl/scope.hpp"

namespace snabl {
	Scope::Scope(Env &env, const ScopePtr &parent):
		env(env), parent(parent), stack_begin(env.stack_begin()) {
	}

	Box const* Scope::get(const Sym &key) {
		auto found(_vars.find(key));
		if (found != _vars.end()) { return &found->second; }
		return parent ? parent->get(key) : nullptr;
	}
}
