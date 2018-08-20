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
	}

	AType::AType(Lib &lib, const Sym &id):
		lib(lib), tag(lib.env.next_type_tag()), id(id) { }
	
	bool AType::isa(const ATypePtr &parent) const {
		return
			parent.get() == this ||
			(_parent_types.size() > parent->tag && _parent_types[parent->tag]);
	}
	
	void AType::dump(const Box &value, std::ostream &out) {
		out << id.name() << "(n/a)";
	}

	Trait::Trait(Lib &lib, const Sym &id): Type<std::nullptr_t>(lib, id) { }
}
