#include "snabl/box.hpp"

namespace snabl {
	const ATypePtr &Box::type() const { return _type; }
	
	void Box::dump(std::ostream &out) const { _type->dump(*this, out); }
}
