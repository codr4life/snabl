#include "snabl/op.hpp"

namespace snabl {
	size_t AOpType::next_label_offs(0);

	AOpType::AOpType(const std::string &id):
		id(id), label_offs(next_label_offs++) { }

	namespace ops {
		const OpType<Begin> Begin::type("Begin");
		const OpType<End> End::type("End");
		const OpType<Funcall> Funcall::type("Funcall");
		const OpType<Push> Push::type("Push");

		Begin::Begin(const ScopePtr &parent): parent(parent) { }

		Funcall::Funcall(const AFuncPtr &func): func(func) { }
		
		Funcall::Funcall(const AFimpPtr &fimp): func(fimp->afunc()), fimp(fimp) { }

		End::End() { }
		Push::Push(const Box &value): value(value) { }
	}
}
