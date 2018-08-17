#include "snabl/box.hpp"

namespace snabl {
	void Box::dump(std::ostream &out) const { _type->dump(*this, out); }
}
