#ifndef SNABL_FUNC_HPP
#define SNABL_FUNC_HPP

#include <memory>

#include "snabl/sym.hpp"

namespace snabl {
	class Func {
	public:
		const Sym id;
		Func(const Sym &id);
	};

	using FuncPtr = std::shared_ptr<Func>;
}

#endif
