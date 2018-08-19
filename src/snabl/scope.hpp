#ifndef SNABL_SCOPE_HPP
#define SNABL_SCOPE_HPP

#include <memory>
#include <unordered_map>

#include "snabl/stack.hpp"

namespace snabl {
	class Env;
	class Scope;
	
	using ScopePtr = std::shared_ptr<Scope>;

	class Scope {
	public:
		Env &env;
		const ScopePtr parent;
		
		Scope(Env &env, const ScopePtr &parent);
		
		Stack::const_iterator stack_begin() const;
		Box pop_stack();
		Box const* get_var(const Sym &key);
		
		template <typename ValueT>
		const Box &set_var(const Sym &key,
											 const TypePtr<ValueT> &type,
											 const ValueT &value);
	private:
		std::unordered_map<Sym, Box> _vars;
		Stack::const_iterator _stack_begin;
	};

	template <typename ValueT>
	const Box &Scope::set_var(const Sym &key,
														const TypePtr<ValueT> &type,
														const ValueT &value) {
		auto found(_vars.find(key));

		if (found != _vars.end()) {
			found->second = Box(type, value);
			return found->second;
		}

		return _vars.emplace(std::make_pair(key, Box(type, value))).first->second;
	}
}

#endif
