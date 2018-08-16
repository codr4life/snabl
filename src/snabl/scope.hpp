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
	};
}

#endif
