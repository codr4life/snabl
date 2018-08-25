#ifndef SNABL_SCOPE_HPP
#define SNABL_SCOPE_HPP

#include <memory>
#include <unordered_map>

#include "snabl/ptrs.hpp"
#include "snabl/stack.hpp"
#include "snabl/sym.hpp"

namespace snabl {
	class Env;

	class Scope {
	public:
		Env &env;
		
		Scope(Env &env);
		
		size_t stack_offs() const;
		Box pop_stack();
		Box const* get_var(const Sym &id);
		void put_var(const Sym &id, const Box &value);
	private:
		std::unordered_map<Sym, Box> _vars;
		size_t _stack_offs;
	};
}

#endif
