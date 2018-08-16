#ifndef SNABL_FUNC_HPP
#define SNABL_FUNC_HPP

#include <memory>

#include "snabl/sym.hpp"

namespace snabl {
	class AFunc {
	public:
		const Sym id;
		const int nargs, nrets;
		
		AFunc(const Sym &id, int nargs, int nrets);
	};

	using AFuncPtr = std::shared_ptr<AFunc>;

	template <int NARGS, int NRETS>
	class Func: public AFunc {
	public:
		Func(const Sym &id);
	};

	template <int NARGS, int NRETS>
	Func<NARGS, NRETS>::Func(const Sym &id): AFunc(id, NARGS, NRETS) {
	}

	template <int NARGS, int NRETS> 
	using FuncPtr = std::shared_ptr<Func<NARGS, NRETS>>;
}


#endif
