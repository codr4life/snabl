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
		const OpType<Eqval> Eqval::type("eqval");
		const OpType<FimpRet> FimpRet::type("fimpret");
		const OpType<Funcall> Funcall::type("funcall");
		const OpType<Get> Get::type("get");
		const OpType<Lambda> Lambda::type("lambda");
		const OpType<LambdaRet> LambdaRet::type("lambdaret");
		const OpType<Let> Let::type("let");
		const OpType<Push> Push::type("push");
		const OpType<Recall> Recall::type("recall");
		const OpType<Rot> Rot::type("rot");
		const OpType<RSwap> RSwap::type("rswap");
		const OpType<SDrop> SDrop::type("sdrop");
		const OpType<Skip> Skip::type("skip");
		const OpType<Swap> Swap::type("swap");

		Funcall::Funcall(const FuncPtr &func): func(func) { }
		Funcall::Funcall(const FimpPtr &fimp): func(fimp->func), fimp(fimp) { }
	}
}
