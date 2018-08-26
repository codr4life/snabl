#ifndef SNABL_MACRO_HPP
#define SNABL_MACRO_HPP

#include "snabl/form.hpp"
#include "snabl/sym.hpp"

namespace snabl {
	class Lib;
	
	class Macro {
	public:
		using Imp = std::function<void (Forms::const_iterator &in,
																		const Forms::const_iterator &end,
																		AFuncPtr &func, AFimpPtr &fimp,
																		Bin &out)>;
		
		Lib &lib;
		const Sym id;

		Macro(Lib &lib, Sym id, const Imp &imp);
		
		void call(Forms::const_iterator &in,
							const Forms::const_iterator &end,
							AFuncPtr &func, AFimpPtr &fimp,
							Bin &out);
	private:
		const Imp _imp;
	};
}

#endif
