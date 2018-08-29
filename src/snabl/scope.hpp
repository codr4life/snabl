#ifndef SNABL_SCOPE_HPP
#define SNABL_SCOPE_HPP

#include <map>
#include <memory>

#include "snabl/ptrs.hpp"
#include "snabl/stack.hpp"
#include "snabl/std.hpp"
#include "snabl/sym.hpp"

namespace snabl {
	class Env;

	class Scope {
	public:
		Env &env;
		
		Scope(Env &env);

		void put_var(Sym id, const optional<Box> &value);
		optional<Box> get_var(Sym id) const;
	private:
		map<Sym, Box> _vars;
	};
}

#endif
