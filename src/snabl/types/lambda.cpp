#include "snabl/env.hpp"
#include "snabl/box.hpp"
#include "snabl/lambda.hpp"
#include "snabl/lib.hpp"
#include "snabl/types/lambda.hpp"

namespace snabl {
	LambdaType::LambdaType(Lib &lib, Sym id): Type<LambdaPtr>(lib, id) { }

	void LambdaType::call(const Box &val, Pos pos) const {
		Lambda::call(val.as<LambdaPtr>(), val.type()->lib.env, pos);
	}
	
	void LambdaType::dump(const Box &val, ostream &out) const {
		auto &l(val.as<LambdaPtr>());
		out << "(Lambda " << l.get() << ')';
	}
}
