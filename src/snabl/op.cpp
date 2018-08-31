#include "snabl/env.hpp"
#include "snabl/fimp.hpp"
#include "snabl/func.hpp"
#include "snabl/op.hpp"

namespace snabl {
	size_t AOpType::next_label_offs(0);

	AOpType::AOpType(const string &id):
		id(id), label_offs(next_label_offs++) { }

	void Op::dump(Env &env, ostream &out) const {
		out << type.id;
		_imp->dump(env, out);
		out << endl;
	}
	
	namespace ops {
		const OpType<Begin> Begin::type("Begin");
		const OpType<Call> Call::type("Call");
		const OpType<Drop> Drop::type("Drop");
		const OpType<Dup> Dup::type("Dup");
		const OpType<Else> Else::type("Else");
		const OpType<End> End::type("End");
		const OpType<FimpRet> FimpRet::type("FimpRet");
		const OpType<Funcall> Funcall::type("Funcall");
		const OpType<GetVar> GetVar::type("GetVar");
		const OpType<Lambda> Lambda::type("Lambda");
		const OpType<LambdaRet> LambdaRet::type("LambdaRet");
		const OpType<Push> Push::type("Push");
		const OpType<PutVar> PutVar::type("PutVar");
		const OpType<Skip> Skip::type("Skip");
		const OpType<Swap> Swap::type("Swap");

		Begin::Begin(ScopePtr parent): parent(parent) { }

		Call::Call() { }

		Drop::Drop() { }

		Dup::Dup() { }

		Else::Else(size_t nops): nops(nops) { }

		void Else::dump(Env &env, ostream &out) const { out << ' ' << nops; }

		End::End() { }
		
		FimpRet::FimpRet(bool end_scope): end_scope(end_scope) { }

		Funcall::Funcall(FuncPtr func): func(func) { }
		
		Funcall::Funcall(FimpPtr fimp): func(fimp->func), fimp(fimp) { }

		void Funcall::dump(Env &env, ostream &out) const {
			out << ' ' << (fimp ? fimp->id.name() : func->id.name());
			if (prev_fimp) { out << ' ' << prev_fimp->id.name(); }
		}

		GetVar::GetVar(Sym id): id(id) { }

		void GetVar::dump(Env &env, ostream &out) const { out << ' ' << id.name(); }

		Lambda::Lambda(): nops(0) { }

		void Lambda::dump(Env &env, ostream &out) const {
			out << ' ' << *start_pc-env.ops.begin() << ':' << nops;
		}
		
		LambdaRet::LambdaRet() { }

		Push::Push(const Box &val): val(val) { }

		void Push::dump(Env &env, ostream &out) const {
			out << ' ';
			val.dump(out);
		}

		PutVar::PutVar(Sym id): id(id) { }

		void PutVar::dump(Env &env, ostream &out) const { out << ' ' << id.name(); }

		Skip::Skip(size_t nops): nops(nops) { }

		void Skip::dump(Env &env, ostream &out) const { out << ' ' << nops; }

		Swap::Swap() { }
	}
}
