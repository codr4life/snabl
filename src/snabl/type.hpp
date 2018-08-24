#ifndef SNABL_TYPE_HPP
#define SNABL_TYPE_HPP

#include <unordered_set>
#include <vector>

#include "snabl/ptrs.hpp"
#include "snabl/sym.hpp"

namespace snabl {
	class Box;
	class Lib;

	class AType {
	public:
		Lib &lib;
		const size_t tag;
		const Sym id;

		static void derive(const ATypePtr &child, const ATypePtr &parent);
		bool isa(const ATypePtr &parent) const;

		virtual bool is_equid(const Box &lhs, const Box &rhs) const=0;
		virtual bool is_eqval(const Box &lhs, const Box &rhs) const=0;
		
		virtual void dump(const Box &value, std::ostream &out) const;
		virtual void write(const Box &value, std::ostream &out) const;
	protected:
		AType(Lib &lib, const Sym &id);
	private:
		std::vector<ATypePtr> _parent_types;
		std::unordered_set<ATypePtr> _child_types;
	};

	template <typename ValueT>
	class Type: public AType {
	public:
		Type(Lib &lib, const Sym &id);

		bool is_equid(const Box &lhs, const Box &rhs) const override;
		bool is_eqval(const Box &lhs, const Box &rhs) const override;
	};

	template <typename ValueT>
	Type<ValueT>::Type(Lib &lib, const Sym &id): AType(lib, id) { }

	template <typename ValueT>
	bool Type<ValueT>::is_equid(const Box &lhs, const Box &rhs) const {
		return is_eqval(lhs, rhs);
	}
	
	template <typename ValueT>
	bool Type<ValueT>::is_eqval(const Box &lhs, const Box &rhs) const {
		return lhs.as<ValueT>() == rhs.as<ValueT>();
	}
	
	class Trait: public Type<std::nullptr_t> {
	public:
		Trait(Lib &lib, const Sym &id);
	};
}

#endif
