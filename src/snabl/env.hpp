#ifndef SNABL_ENV_HPP
#define SNABL_ENV_HPP

#include <deque>
#include <unordered_map>
#include <vector>

#include "snabl/bin.hpp"
#include "snabl/int.hpp"
#include "snabl/lib.hpp"
#include "snabl/scope.hpp"
#include "snabl/stack.hpp"
#include "snabl/sym.hpp"

namespace snabl {
	template <typename ValueT>
	class Type;
	
	class Env {
	private:
		std::vector<ScopePtr> _scopes;
		std::unordered_map<std::string, std::unique_ptr<SymImp>> _syms;
	public:
		Bin bin;
		Lib lobby;
		const TypePtr<Int> int_type;
		const ScopePtr main;
		
		Env();
		Sym get_sym(const std::string &name);

		ScopePtr begin(const ScopePtr &parent=nullptr);
		ScopePtr scope();
		ScopePtr end();
		
		template <typename ValueT>
		void push_stack(const TypePtr<ValueT> &type, const ValueT &value);
		std::optional<Box> pop_stack();
		
		Stack::iterator stack_end();
	private:
		Stack _stack;
	};

	template <typename ValueT>
	void Env::push_stack(const TypePtr<ValueT> &type, const ValueT &value) {
		_stack.emplace_back(type, value);
	}
}

#endif
