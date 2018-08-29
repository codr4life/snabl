#include "snabl/fimp.hpp"
#include "snabl/func.hpp"
#include "snabl/op.hpp"

namespace snabl {
	size_t AOpType::next_label_offs(0);

	AOpType::AOpType(const string &id):
		id(id), label_offs(next_label_offs++) { }

	void Op::dump(ostream &out) const {
		out << type.id;
		if (_imp) { _imp->dump(out); }
		out << endl;
	}
	
	namespace ops {
		const OpType<Begin> Begin::type("Begin");
		const OpType<Drop> Drop::type("Drop");
		const OpType<Else> Else::type("Else");
		const OpType<End> End::type("End");
		const OpType<Fimp> Fimp::type("Fimp");
		const OpType<Funcall> Funcall::type("Funcall");
		const OpType<GetVar> GetVar::type("GetVar");
		const OpType<Push> Push::type("Push");
		const OpType<PutVar> PutVar::type("PutVar");
		const OpType<Return> Return::type("Return");
		const OpType<Skip> Skip::type("Skip");

		Begin::Begin() { }

		Drop::Drop() { }
		
		Else::Else(size_t nops): nops(nops) { }

		void Else::dump(ostream &out) const { out << ' ' << nops; }

		End::End() { }
		
		Fimp::Fimp(const FimpPtr &ptr): ptr(ptr) { }

		void Fimp::dump(ostream &out) const { out << ' ' << ptr->id.name(); }

		Funcall::Funcall(const FuncPtr &func): func(func) { }
		
		Funcall::Funcall(const FimpPtr &fimp): func(fimp->func), fimp(fimp) { }

		void Funcall::dump(ostream &out) const {
			out << ' ' << (fimp ? fimp->id.name() : func->id.name());
			if (prev_fimp) { out << ' ' << prev_fimp->id.name(); }
		}

		GetVar::GetVar(Sym id): id(id) { }

		void GetVar::dump(ostream &out) const { out << ' ' << id.name(); }

		Push::Push(const Box &value): value(value) { }

		void Push::dump(ostream &out) const {
			out << ' ';
			value.dump(out);
		}

		PutVar::PutVar(Sym id): id(id) { }

		void PutVar::dump(ostream &out) const { out << ' ' << id.name(); }

		Return::Return() { }

		Skip::Skip(size_t nops): nops(nops) { }

		void Skip::dump(ostream &out) const { out << ' ' << nops; }
	}
}
