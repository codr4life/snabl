#include "snabl/env.hpp"
#include "snabl/fimp.hpp"
#include "snabl/lib.hpp"

namespace snabl {
	void AFimp::call(const AFimpPtr &fimp) {
		auto &env(fimp->afunc()->lib.env);
						 
		if (fimp->_imp) {
			auto &call(env.push_call(fimp, -1));
			(*fimp->_imp)(call);
			env.pop_call();
		}
	}

	AFimp::AFimp(const Sym &id, Imp imp): id(id), _imp(imp) { }
	
	AFimp::AFimp(const Sym &id, Forms &&forms): id(id), forms(std::move(forms)) { }
	
	void AFimp::dump(std::ostream &out) const { out << id.name(); }
}
