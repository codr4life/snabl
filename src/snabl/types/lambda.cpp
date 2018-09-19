#include <ostream>

#include "snabl/call.hpp"
#include "snabl/env.hpp"
#include "snabl/box.hpp"
#include "snabl/lambda.hpp"
#include "snabl/lib.hpp"
#include "snabl/types/lambda.hpp"

namespace snabl {
	LambdaType::LambdaType(Lib &lib, Sym id): Type<LambdaPtr>(lib, id) { }

	void LambdaType::call(const Box &val, Pos pos, bool now) const {
		Lambda::call(val.as<LambdaPtr>(), val.type()->lib.env, pos, now);
	}
	
	void LambdaType::dump(const Box &val, ostream &out) const {
		auto &l(val.as<LambdaPtr>());
		out << "Lambda(" << l.get() << ')';
	}
}
