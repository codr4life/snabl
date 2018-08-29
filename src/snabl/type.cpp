#include <ostream>

#include "snabl/env.hpp"
#include "snabl/lib.hpp"
#include "snabl/type.hpp"

namespace snabl {
	void AType::derive(const ATypePtr &child, const ATypePtr &parent) {
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

	AType::AType(Lib &lib, Sym id):
		lib(lib), tag(lib.env.next_type_tag()), id(id) { }
	
	bool AType::isa(const ATypePtr &parent) const {
		return
			parent.get() == this ||
			(_parent_types.size() > parent->tag && _parent_types[parent->tag]);
	}
	
	void AType::dump(const Box &value, ostream &out) const {
		out << id.name() << "(n/a)";
	}

	void AType::print(const Box &value, ostream &out) const { dump(value, out); }

	void AType::write(const Box &value, ostream &out) const { print(value, out); }

	Trait::Trait(Lib &lib, Sym id): Type<nullptr_t>(lib, id) { }
}
