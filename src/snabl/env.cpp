#include "snabl/env.hpp"

namespace snabl {
	Env::Env():
		lobby(get_sym("lobby")),
		int_type(lobby.add_type<IntType>(get_sym("Int"))),
		bin(*this),
		main(begin(nullptr)) { }

	Sym Env::get_sym(const std::string &name) {
		auto found(_syms.find(name));

		return Sym((found == _syms.end())
							 ? _syms.emplace(name,
															 std::make_unique<SymImp>(name)).first->second.get()
							 : found->second.get());
	}

	ScopePtr Env::begin(const ScopePtr &parent) {
		auto s(std::make_shared<Scope>(*this, parent));
		_scopes.push_back(s);
		return s;
	}

	ScopePtr Env::scope() { return _scopes.back(); }

	ScopePtr Env::end() {
		auto s(_scopes.back());
		_scopes.pop_back();
		return s;
	}

	void Env::push_stack(const Box &value) { _stack.push_back(value); }

	std::optional<Box> Env::pop_stack() {
		if (_stack.empty()) { return std::nullopt; }
		Box v(_stack.back());
		_stack.pop_back();
		return v;
	}

	Stack::iterator Env::stack_end() { return _stack.end(); }
}
