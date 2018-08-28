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

		void put_var(Sym id, const Box &value);
	private:
		std::map<Sym, Box> _put_vars;
	};
}

#endif
