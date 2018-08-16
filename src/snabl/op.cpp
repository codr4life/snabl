#include "snabl/op.hpp"

namespace snabl {
	OpType::OpType(const std::string &id, size_t label_offs):
		id(id), label_offs(label_offs) { }

	namespace ops {
		const OpType Push::type("Push", 0);

		Push::Push(const Box &value): value(value) { }
	}

	Op::Op(const OpType &type): type(type) { }

	Op::~Op() {}
}
