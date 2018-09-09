#ifndef SNABL_TYPE_HPP
#define SNABL_TYPE_HPP

#include "snabl/box.hpp"
#include "snabl/cmp.hpp"
#include "snabl/def.hpp"
#include "snabl/ptrs.hpp"
#include "snabl/std.hpp"

namespace snabl {
	class Box;
	class Lib;

	class AType: public Def {
	public:
		Lib &lib;
		const size_t tag;

		static void derive(const ATypePtr &child, const ATypePtr &parent) {
			if (child->_parent_types.size() <= parent->tag) {
				child->_parent_types.resize(parent->tag+1);
			}
			
			child->_parent_types[parent->tag] = parent;
			for (auto &c: child->_child_types) { derive(c, parent); }
			parent->_child_types.insert(child);
			
			for (auto &p: parent->_parent_types) {
				if (p) { derive(child, p); }
			}
		}

		bool isa(const ATypePtr &parent) const {
			return
				parent.get() == this ||
				(_parent_types.size() > parent->tag && _parent_types[parent->tag]);
		}

		virtual bool equid(const Box &lhs, const Box &rhs) const=0;
		virtual bool eqval(const Box &lhs, const Box &rhs) const=0;
		virtual Cmp cmp(const Box &lhs, const Box &rhs) const=0;
		virtual bool is_true(const Box &val) const { return true; }
		virtual void call(const Box &val, Pos pos, bool now) const;

		virtual void dump(const Box &val, ostream &out) const {
			out << id.name() << "(n/a)";
		}

		void print(const Box &val, ostream &out) const { dump(val, out); }

		void write(const Box &val, ostream &out) const { print(val, out); }
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

		virtual bool equid(const Box &lhs, const Box &rhs) const override;
		virtual bool eqval(const Box &lhs, const Box &rhs) const override;
		virtual Cmp cmp(const Box &lhs, const Box &rhs) const override;
	};

	template <typename ValT>
	Type<ValT>::Type(Lib &lib, Sym id): AType(lib, id) { }

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
