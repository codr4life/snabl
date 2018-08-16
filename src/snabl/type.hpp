#ifndef SNABL_TYPE_HPP
#define SNABL_TYPE_HPP

#include "snabl/sym.hpp"

namespace snabl {
	class AType {
	protected:
		AType(const Sym &id);
	private:
		const Sym _id;
	};

	using ATypePtr = std::shared_ptr<AType>;

	template <typename ValueT>
	class Type: public AType {
	public:
		Type(const Sym &id);
	};

	template <typename ValueT>
	Type<ValueT>::Type(const Sym &id): AType(id) {
	}
}

#endif
