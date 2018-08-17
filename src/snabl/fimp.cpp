#include "snabl/fimp.hpp"

namespace snabl {
	AFimp::AFimp(const Sym &id, Imp imp): id(id), imp(imp) { }

	void AFimp::dump(std::ostream &out) const { out << id.name(); }
}
