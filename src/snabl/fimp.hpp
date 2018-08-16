#ifndef SNABL_FIMP_HPP
#define SNABL_FIMP_HPP

#include <memory>

#include "snabl/func.hpp"

namespace snabl {
	class Fimp {
	public:
		Fimp(const AFuncPtr &func);
	private:
		const AFuncPtr _func;
	};

	using FimpPtr = std::shared_ptr<Fimp>;
}

#endif
