#ifndef SNABL_FIMP_HPP
#define SNABL_FIMP_HPP

#include <memory>

#include "snabl/func.hpp"

namespace snabl {
	class AFimp {
	public:
		AFimp(const FuncPtr &func);
	private:
		const FuncPtr _func;
	};

	using AFimpPtr = std::shared_ptr<AFimp>;

	template <size_t NARGS>
	class Fimp: public AFimp {
	public:
		Fimp(const FuncPtr &func);
	};

	template <size_t NARGS>
	Fimp<NARGS>::Fimp(const FuncPtr &func): AFimp(func) {
	}
}

#endif
