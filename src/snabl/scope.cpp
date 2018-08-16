#include "snabl/env.hpp"
#include "snabl/scope.hpp"

namespace snabl {
	Scope::Scope(Env &env, const ScopePtr &parent):
		env(env), parent(parent), stack_begin(env.stack_begin()) {
	}
}
