#ifndef SNABL_TYPE_HPP
#define SNABL_TYPE_HPP

#include "snabl/atype.hpp"
#include "snabl/box.hpp"

namespace snabl {
	template <typename ValT>
	class Type: public AType {
	public:
		Type(Lib &lib, Sym id);

		virtual bool equid(const Box &lhs, const Box &rhs) const override;
		virtual bool eqval(const Box &lhs, const Box &rhs) const override;
		virtual Cmp cmp(const Box &lhs, const Box &rhs) const override;
	};

	template <typename ValT>
	Type<ValT>::Type(Lib &lib, Sym id): AType(lib, id, sizeof(ValT)) { }

	template <typename ValT>
	bool Type<ValT>::equid(const Box &lhs, const Box &rhs) const {
		return eqval(lhs, rhs);
	}
	
	template <typename ValT>
	bool Type<ValT>::eqval(const Box &lhs, const Box &rhs) const {
		return lhs.as<ValT>() == rhs.as<ValT>();
	}
	
	template <typename ValT>
	Cmp Type<ValT>::cmp(const Box &lhs, const Box &rhs) const {
		return snabl::cmp(lhs.as<ValT>(), rhs.as<ValT>());
	}

	class Trait: public Type<nullptr_t> {
	public:
		Trait(Lib &lib, Sym id): Type<nullptr_t>(lib, id) { }
	};
}

#endif
