#include "snabl/op.hpp"

namespace snabl {
	size_t OpType::next_label_offs(0);

	OpType::OpType(const std::string &id):
		id(id), label_offs(next_label_offs++) { }

	namespace ops {
		const OpType Begin::type("Begin");
		const OpType End::type("End");
		const OpType Push::type("Push");

		Begin::Begin(const ScopePtr &parent): parent(parent) { }
		End::End() { }
		Push::Push(const Box &value): value(value) { }
	}

	Op::~Op() {}
}
