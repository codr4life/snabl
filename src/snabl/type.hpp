#ifndef SNABL_TYPE_HPP
#define SNABL_TYPE_HPP

#include "snabl/atype.hpp"
#include "snabl/box.hpp"

namespace snabl {
	template <typename T>
	class Type: public AType {
	public:
		static_assert(sizeof(T) <= AType::MaxSize);

		Type(Lib &lib, Sym id): AType(lib, id, sizeof(T)) { }

		void copy(optional<Var<MaxSize>> &dst, const Var<MaxSize> &src) const override {
			dst.emplace(src.as<T>());
		}
		
		void destroy(const Var<MaxSize> &val) const override { val.as<T>().~T(); }

		bool equid(const Box &lhs, const Box &rhs) const override {
			return eqval(lhs, rhs);
		}
		
		bool eqval(const Box &lhs, const Box &rhs) const override {
			return lhs.as<T>() == rhs.as<T>();
		}
		
		Cmp cmp(const Box &lhs, const Box &rhs) const override {
			return snabl::cmp(lhs.as<T>(), rhs.as<T>());
		}
	};

	class Trait: public Type<nullptr_t> {
	public:
		Trait(Lib &lib, Sym id): Type<nullptr_t>(lib, id) { }
	};
}

#endif
