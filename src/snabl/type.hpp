#ifndef SNABL_TYPE_HPP
#define SNABL_TYPE_HPP

#include <unordered_set>

#include "snabl/cmp.hpp"
#include "snabl/ptrs.hpp"
#include "snabl/std.hpp"
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
		virtual Cmp cmp(const Box &lhs, const Box &rhs) const=0;
		
		virtual void dump(const Box &value, ostream &out) const;
		virtual void print(const Box &value, ostream &out) const;
		virtual void write(const Box &value, ostream &out) const;
	protected:
		AType(Lib &lib, Sym id);
	private:
		vector<ATypePtr> _parent_types;
		unordered_set<ATypePtr> _child_types;
	};

	template <typename ValueT>
	class Type: public AType {
	public:
		Type(Lib &lib, Sym id);

		bool is_equid(const Box &lhs, const Box &rhs) const override;
		bool is_eqval(const Box &lhs, const Box &rhs) const override;
		virtual Cmp cmp(const Box &lhs, const Box &rhs) const override;
	};

	template <typename ValueT>
	Type<ValueT>::Type(Lib &lib, Sym id): AType(lib, id) { }

	template <typename ValueT>
	bool Type<ValueT>::is_equid(const Box &lhs, const Box &rhs) const {
		return is_eqval(lhs, rhs);
	}
	
	template <typename ValueT>
	bool Type<ValueT>::is_eqval(const Box &lhs, const Box &rhs) const {
		return lhs.as<ValueT>() == rhs.as<ValueT>();
	}
	
	template <typename ValueT>
	Cmp Type<ValueT>::cmp(const Box &lhs, const Box &rhs) const {
		return snabl::cmp(lhs.as<ValueT>(), rhs.as<ValueT>());
	}

	class Trait: public Type<nullptr_t> {
	public:
		Trait(Lib &lib, Sym id);
	};
}

#endif
