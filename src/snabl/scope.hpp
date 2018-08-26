#ifndef SNABL_SCOPE_HPP
#define SNABL_SCOPE_HPP

#include <map>
#include <memory>

#include "snabl/ptrs.hpp"
#include "snabl/stack.hpp"
#include "snabl/sym.hpp"

namespace snabl {
	class Env;

	class Scope {
	public:
		Env &env;
		
		Scope(Env &env);
		~Scope();
		
		size_t stack_offs() const;
		Box pop_stack();
		void put_var(const Sym &id, const Box &value);
	private:
		std::map<Sym, Box> _put_vars;
		size_t _stack_offs;
	};
}

#endif
