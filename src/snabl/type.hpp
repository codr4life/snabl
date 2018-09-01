#ifndef SNABL_TYPE_HPP
#define SNABL_TYPE_HPP

#include "snabl/box.hpp"
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
		virtual bool is_true(const Box &val) const;
		virtual void call(const Box &val, bool now) const; 
		virtual void dump(const Box &val, ostream &out) const;
		virtual void print(const Box &val, ostream &out) const;
		virtual void write(const Box &val, ostream &out) const;
	protected:
		AType(Lib &lib, Sym id);
	private:
		vector<ATypePtr> _parent_types;
		unordered_set<ATypePtr> _child_types;
	};

	template <typename ValT>
	class Type: public AType {
	public:
		Type(Lib &lib, Sym id);

		virtual bool is_equid(const Box &lhs, const Box &rhs) const override;
		virtual bool is_eqval(const Box &lhs, const Box &rhs) const override;
		virtual Cmp cmp(const Box &lhs, const Box &rhs) const override;
	};

	template <typename ValT>
	Type<ValT>::Type(Lib &lib, Sym id): AType(lib, id) { }

	template <typename ValT>
	bool Type<ValT>::is_equid(const Box &lhs, const Box &rhs) const {
		return is_eqval(lhs, rhs);
	}
	
	template <typename ValT>
	bool Type<ValT>::is_eqval(const Box &lhs, const Box &rhs) const {
		return lhs.as<ValT>() == rhs.as<ValT>();
	}
	
	template <typename ValT>
	Cmp Type<ValT>::cmp(const Box &lhs, const Box &rhs) const {
		return snabl::cmp(lhs.as<ValT>(), rhs.as<ValT>());
	}

	class Trait: public Type<nullptr_t> {
	public:
		Trait(Lib &lib, Sym id);
	};
}

#endif
