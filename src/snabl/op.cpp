#include "snabl/op.hpp"

namespace snabl {	
	Op::Op(const OpType &type, const std::any &data):
		type(type), _data(data) {
	}

	namespace ops {
		static const OpType Push::type("Push", 0);

		Push::Push(const Box &value): value(value);
	}
}
