#ifndef SNABL_SCOPE_HPP
#define SNABL_SCOPE_HPP

#include <memory>
#include "snabl/stack.hpp"

namespace snabl {
	class Env;
	class Scope;
	
	using ScopePtr = std::shared_ptr<Scope>;

	class Scope {
	public:
		Env &env;
		const ScopePtr parent;
		const Stack::iterator stack_begin;
		
		Scope(Env &env, const ScopePtr &parent);
		
		Box const* get(const Sym &key); 
		template <typename ValueT>
		const Box &set(const Sym &key, const TypePtr<ValueT> &type, const ValueT &value);
	private:
		std::unordered_map<Sym, Box> _vars;
	};

	template <typename ValueT>
	const Box &Scope::set(const Sym &key,
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
