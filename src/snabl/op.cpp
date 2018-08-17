#include "snabl/op.hpp"

namespace snabl {
	size_t AOpType::next_label_offs(0);

	AOpType::AOpType(const std::string &id):
		id(id), label_offs(next_label_offs++) { }

	namespace ops {
		const OpType<Begin> Begin::type("Begin");
		const OpType<End> End::type("End");
		const OpType<Push> Push::type("Push");

		Begin::Begin(const ScopePtr &parent): parent(parent) { }
		End::End() { }
		Push::Push(const Box &value): value(value) { }
	}
}
