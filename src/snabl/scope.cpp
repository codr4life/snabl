#include "snabl/env.hpp"
#include "snabl/error.hpp"
#include "snabl/scope.hpp"

namespace snabl {
	Scope::Scope(Env &env, const ScopePtr &parent):
		env(env), parent(parent), _stack_begin(env.stack().end()) { }

	Stack::const_iterator Scope::stack_begin() const { return _stack_begin; }
	
	Box Scope::pop_stack() {
		if (env.stack().end() <= _stack_begin) { throw Error("Nothing to pop"); }
		return *env.pop_stack();
	}

	Box const* Scope::get_var(const Sym &key) {
		auto found(_vars.find(key));
		if (found != _vars.end()) { return &found->second; }
		return parent ? parent->get_var(key) : nullptr;
	}
}
