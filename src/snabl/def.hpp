#ifndef SNABL_DEF_HPP
#define SNABL_DEF_HPP

#include "snabl/sym.hpp"

namespace snabl {
	class Def {
	public:
		const Sym id;
	protected:
		Def(Sym id): id(id) { }
	};

	inline ostream &operator <<(ostream &out, const Def &x) {
		out << x.id;
		return out;
	}
}

#endif
