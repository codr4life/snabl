#ifndef SNABL_MACRO_HPP
#define SNABL_MACRO_HPP

#include "snabl/form.hpp"
#include "snabl/std.hpp"
#include "snabl/sym.hpp"

namespace snabl {
	class Lib;
	
	class Macro {
	public:
		using Imp = function<void (Forms::const_iterator &in,
															 Forms::const_iterator end,
															 FuncPtr &func, FimpPtr &fimp,
															 Env &env)>;
		
		Lib &lib;
		const Sym id;

		Macro(Lib &lib, Sym id, const Imp &imp);
		
		void call(Forms::const_iterator &in,
							Forms::const_iterator end,
							FuncPtr &func, FimpPtr &fimp,
							Env &env);
	private:
		const Imp _imp;
	};
}

#endif
