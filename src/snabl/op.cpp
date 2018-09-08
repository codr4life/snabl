#include "snabl/env.hpp"
#include "snabl/fimp.hpp"
#include "snabl/func.hpp"
#include "snabl/op.hpp"

namespace snabl {
	size_t AOpType::next_label_offs(0);

	namespace ops {
		const OpType<Call> Call::type("Call");
		const OpType<DDrop> DDrop::type("DDrop");
		const OpType<Drop> Drop::type("Drop");
		const OpType<Dup> Dup::type("Dup");
		const OpType<Else> Else::type("Else");
		const OpType<FimpRet> FimpRet::type("FimpRet");
		const OpType<Funcall> Funcall::type("Funcall");
		const OpType<Get> Get::type("Get");
		const OpType<Lambda> Lambda::type("Lambda");
		const OpType<LambdaRet> LambdaRet::type("LambdaRet");
		const OpType<Let> Let::type("Let");
		const OpType<Push> Push::type("Push");
		const OpType<Recall> Recall::type("Recall");
		const OpType<Rot> Rot::type("Rot");
		const OpType<RSwap> RSwap::type("RSwap");
		const OpType<SDrop> SDrop::type("SDrop");
		const OpType<Skip> Skip::type("Skip");
		const OpType<Swap> Swap::type("Swap");

		Funcall::Funcall(const FuncPtr &func): func(func) { }
		Funcall::Funcall(const FimpPtr &fimp): func(fimp->func), fimp(fimp) { }
	}
}
