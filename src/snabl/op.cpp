#include "snabl/env.hpp"
#include "snabl/fimp.hpp"
#include "snabl/func.hpp"
#include "snabl/op.hpp"

namespace snabl {
	size_t AOpType::next_label_offs(0);

	namespace ops {
		const OpType<Call> Call::type("call");
		const OpType<DDrop> DDrop::type("ddrop");
		const OpType<Drop> Drop::type("drop");
		const OpType<Dup> Dup::type("dup");
		const OpType<Else> Else::type("else");
		const Eqval::Type Eqval::type("eqval");
		const Fimp::Type Fimp::type("fimp");
		const OpType<FimpRet> FimpRet::type("fimpret");
		const Funcall::Type Funcall::type("funcall");
		const OpType<Get> Get::type("get");
		const OpType<Lambda> Lambda::type("lambda");
		const OpType<LambdaRet> LambdaRet::type("lambdaret");
		const OpType<Let> Let::type("let");
		const OpType<Nop> Nop::type("nop");
		const Push::Type Push::type("push");
		const OpType<Recall> Recall::type("recall");
		const OpType<Rot> Rot::type("rot");
		const OpType<RSwap> RSwap::type("rswap");
		const OpType<SDrop> SDrop::type("sdrop");
		const Skip::Type Skip::type("skip");
		const OpType<Swap> Swap::type("swap");
		const OpType<Try> Try::type("try");

		Funcall::Funcall(const FuncPtr &func): func(func) { }
		Funcall::Funcall(const FimpPtr &fimp): func(fimp->func), fimp(fimp) { }

		void Eqval::Type::dump(const Eqval &op, ostream &out) const {
			out << ' ';
			op.lhs.dump(out);
		}

		void Fimp::Type::dump(const Fimp &op, ostream &out) const {
			out << ' ' << op.ptr->id << ' ' << op.ptr->nops();
		}

		void Funcall::Type::dump(const Funcall &op, ostream &out) const {
			out << ' ' << (op.fimp ? op.fimp->id : op.func->id);
			if (op.prev_fimp) { out << " (" << op.prev_fimp->id << ')'; }
		}

		void Push::Type::dump(const Push &op, ostream &out) const {
			out << ' ';
			op.val.dump(out);
		}
	}
}
