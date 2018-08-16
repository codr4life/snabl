#include "snabl/op.hpp"

namespace snabl {
	OpType::OpType(const std::string &id, size_t label_offs):
		id(id), label_offs(label_offs) {
	}

	Op::Op(const OpType &type, const std::any &data):
		type(type), _data(data) {
	}

	namespace ops {
		static const OpType PUSH("Push", 0);

		Push::Push(const Box &value): value(value) {
		}
	}
}
