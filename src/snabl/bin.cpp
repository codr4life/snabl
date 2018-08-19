#include "snabl/bin.hpp"
#include "snabl/env.hpp"

#define SNABL_DISPATCH()																		\
	if (_pc == _ops.end()) { return; }												\
	goto *op_labels[_pc->type.label_offs];										\

namespace snabl {
	BinFimp::BinFimp(size_t begin, size_t end):
		begin(begin), end(end) { }
	
	Bin::Bin(Env &env): env(env) { }

	const Ops &Bin::ops() const { return _ops; }

	std::optional<BinFimp> Bin::get_fimp(const AFimpPtr &ptr) {
		auto found = _fimps.find(ptr);
		return (found == _fimps.end())
			? std::nullopt
			: std::make_optional(found->second);
	}

	void Bin::compile(const Forms &forms) {
		AFuncPtr func;
		AFimpPtr fimp;

		for (auto i(forms.begin()); i != forms.end();) {
			i->type.compile(i, forms.end(), func, fimp, *this);
		}

		if (fimp) {
			emplace_back(ops::Funcall::type, fimp);
		} else if (func) {
			emplace_back(ops::Funcall::type, func);
		}
	}
	
	void Bin::run(size_t offs) {
		_pc = _ops.begin();
		if (offs) { std::advance(_pc, offs); }
		
		static void* op_labels[] = {
			&&op_begin, &&op_drop, &&op_end, &&op_funcall, &&op_push
		};

		SNABL_DISPATCH();
	op_begin:
		env.begin(_pc->as<ops::Begin>().parent);
		_pc++;
		SNABL_DISPATCH();
	op_drop:
		env.pop_stack();
		_pc++;
		SNABL_DISPATCH();
	op_end:
		env.end();
		_pc++;
		SNABL_DISPATCH();
	op_funcall:
		auto &op(_pc->as<ops::Funcall>());
		auto &fimp(op.fimp);
		
		if (!fimp && op.prev_fimp) { fimp = op.prev_fimp; }

		if (fimp) {
			if (fimp->score() == -1) { fimp = nullptr; }
		} else {
			fimp = op.func->get_best_fimp();
		}

		if (!fimp) { throw Error("Func not applicable"); }
		if (!op.fimp) { op.prev_fimp = fimp; }
		AFimp::call(fimp);
		_pc++;
		SNABL_DISPATCH();
	op_push:
		env.push_stack(_pc->as<ops::Push>().value); 
		_pc++;
		SNABL_DISPATCH();
	}
}
