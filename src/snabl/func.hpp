#ifndef SNABL_FUNC_HPP
#define SNABL_FUNC_HPP

#include <memory>

#include "snabl/sym.hpp"

namespace snabl {
	class AFunc {
	public:
		const Sym id;
		AFunc(const Sym &id);
	};

	using AFuncPtr = std::shared_ptr<AFunc>;

	template <int NARGS, int NRETS>
	class Func: public AFunc {
	public:
		Func(const Sym &id);
	};

	template <int NARGS, int NRETS>
	Func<NARGS, NRETS>::Func(const Sym &id): AFunc(id) {
	}

	template <int NARGS, int NRETS> 
	using FuncPtr = std::shared_ptr<Func<NARGS, NRETS>>;
}


#endif
