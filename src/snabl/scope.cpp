#include "snabl/env.hpp"
#include "snabl/error.hpp"
#include "snabl/scope.hpp"

namespace snabl {
	Scope::Scope(Env &env):
		env(env), _stack_offs(env.stack().size()) { }

	size_t Scope::stack_offs() const { return _stack_offs; }
	
	Box Scope::pop_stack() {
		if (env.stack().size() <= _stack_offs) { throw Error("Nothing to pop"); }
		return *env.pop_stack();
	}

	Box const* Scope::get_var(const Sym &id) {
		auto found(_vars.find(id));
		if (found != _vars.end()) { return &found->second; }
		return nullptr;
	}

	void Scope::put_var(const Sym &id, const Box &value) {
		auto found(_vars.find(id));

		if (found != _vars.end()) {
			found->second = value;
		} else {
			_vars.emplace(std::make_pair(id, value)).first->second;
		}
	}
}
