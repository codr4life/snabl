#include "snabl/op.hpp"

namespace snabl {
	std::size_t AOpType::next_label_offs(0);

	AOpType::AOpType(const std::string &id):
		id(id), label_offs(next_label_offs++) { }

	namespace ops {
		const OpType<Begin> Begin::type("Begin");
		const OpType<Drop> Drop::type("Drop");
		const OpType<Else> Else::type("Else");
		const OpType<End> End::type("End");
		const OpType<Funcall> Funcall::type("Funcall");
		const OpType<GetVar> GetVar::type("GetVar");
		const OpType<Push> Push::type("Push");
		const OpType<PutVar> PutVar::type("PutVar");
		const OpType<Skip> Skip::type("Skip");

		Begin::Begin() { }

		Drop::Drop() { }
		
		Else::Else(std::size_t nops): nops(nops) { }

		End::End() { }

		Funcall::Funcall(const AFuncPtr &func): func(func) { }
		
		Funcall::Funcall(const AFimpPtr &fimp): func(fimp->afunc()), fimp(fimp) { }

		GetVar::GetVar(Sym id): id(id) { }

		Push::Push(const Box &value): value(value) { }

		PutVar::PutVar(Sym id): id(id) { }

		Skip::Skip(std::size_t nops): nops(nops) { }
	}
}
