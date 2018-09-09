#include "snabl/box.hpp"
#include "snabl/env.hpp"
#include "snabl/run.hpp"
#include "snabl/types/error.hpp"

namespace snabl {
	ErrorType::ErrorType(Lib &lib, Sym id): Type<ErrorPtr>(lib, id) { }

	bool ErrorType::is_true(const Box &val) const {
		return val.as<ErrorPtr>()->val.is_true();
	}

	void ErrorType::dump(const Box &val, ostream &out) const {
		out << "Error(" << val.as<ErrorPtr>()->what() << ')';
	}
}
