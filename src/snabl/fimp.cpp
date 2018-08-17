#include "snabl/fimp.hpp"

namespace snabl {
	AFimp::AFimp(const Sym &id, Imp imp): id(id), _imp(imp) { }
	
	AFimp::AFimp(const Sym &id, Forms &&forms): id(id), forms(std::move(forms)) { }

	void AFimp::dump(std::ostream &out) const { out << id.name(); }
}
