#ifndef SNABL_FUNC_HPP
#define SNABL_FUNC_HPP

#include <memory>

#include "snabl/sym.hpp"

namespace snabl {
	class AFunc {
	protected:
		AFunc(const Sym &id);
	private:
		const Sym _id;
	};
	
	using AFuncPtr = std::shared_ptr<AFunc>;

	template <size_t NARGS>
	class Func: public AFunc {
	public:
		Func(const Sym &id);
	};

	template <size_t NARGS>
	Func<NARGS>::Func(const Sym &id): AFunc(id) {
	}
}

#endif
