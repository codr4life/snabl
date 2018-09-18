#ifndef SNABL_ATYPE_HPP
#define SNABL_ATYPE_HPP

#include "snabl/cmp.hpp"
#include "snabl/def.hpp"
#include "snabl/error.hpp"
#include "snabl/fmt.hpp"
#include "snabl/ptrs.hpp"
#include "snabl/pos.hpp"
#include "snabl/std.hpp"

namespace snabl {
	class Box;
	class Lib;

	class AType: public Def {
	public:
		Lib &lib;
		const size_t size;
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

		virtual ~AType() { }
		
		bool isa(const ATypePtr &parent) const {
			return
				parent.get() == this ||
				(_parent_types.size() > parent->tag && _parent_types[parent->tag]);
		}

		virtual bool equid(const Box &lhs, const Box &rhs) const=0;
		virtual bool eqval(const Box &lhs, const Box &rhs) const=0;
		virtual Cmp cmp(const Box &lhs, const Box &rhs) const=0;
		virtual bool as_bool(const Box &val) const { return true; }
		virtual void call(const Box &val, Pos pos, bool now) const;

		virtual IterPtr iter(const Box &val) const {
			throw Error(fmt("Invalid seq: %0", {val}));
		}
		
		virtual void dump(const Box &val, ostream &out) const {
			out << id.name() << "(n/a)";
		}

		void print(const Box &val, ostream &out) const { dump(val, out); }

		void write(const Box &val, ostream &out) const { print(val, out); }
	protected:
		AType(Lib &lib, Sym id, size_t size);
	private:
		vector<ATypePtr> _parent_types;
		unordered_set<ATypePtr> _child_types;
	};
}

#endif
