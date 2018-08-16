#ifndef SNABL_FIMP_HPP
#define SNABL_FIMP_HPP

#include <memory>

#include "snabl/func.hpp"

namespace snabl {
	class AFimp {
	public:
		AFimp();
	};

	using AFimpPtr = std::shared_ptr<AFimp>;

	template <int NARGS, int NRETS>
	class Fimp: public AFimp {
	public:
		const FuncPtr<NARGS, NRETS> func;
		Fimp(const FuncPtr<NARGS, NRETS> &func);
	};

	template <int NARGS, int NRETS>
	Fimp<NARGS, NRETS>::Fimp(const FuncPtr<NARGS, NRETS> &func): AFimp(), func(func) {
	}

	template <int NARGS, int NRETS>
	using FimpPtr = std::shared_ptr<Fimp<NARGS, NRETS>>;

}

#endif
