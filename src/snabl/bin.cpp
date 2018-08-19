#include "snabl/bin.hpp"
#include "snabl/env.hpp"

#define SNABEL_DISPATCH()																		\
	if (_pc == _ops.end()) { return; }												\
	goto *op_labels[_pc->type.label_offs];										\

namespace snabl {
	BinFimp::BinFimp(size_t begin, size_t end):
		begin(begin), end(end) { }
	
	Bin::Bin(Env &env): env(env), _pc(_ops.begin()) { }

	std::optional<BinFimp> Bin::get_fimp(const AFimpPtr &ptr) {
		auto found = _fimps.find(ptr);
		return (found == _fimps.end())
			? std::nullopt
			: std::make_optional(found->second);
	}

	Op &Bin::emit_begin(const ScopePtr &parent) {
		return emit_op(ops::Begin::type, parent);
	}

	Op &Bin::emit_end() { return emit_op(ops::End::type); }
	
	Op &Bin::emit_funcall(const AFuncPtr &func) {
		return emit_op(ops::Funcall::type, func);
	}
	
	Op &Bin::emit_funcall(const AFimpPtr &fimp) {
		return emit_op(ops::Funcall::type, fimp);
	}

	Op &Bin::emit_push(const Box &value) { return emit_op(ops::Push::type, value); }

	void Bin::compile(const Forms &forms) {
		for (auto i(forms.begin()); i != forms.end();) {
			i->type.compile(i, forms.end(), *this);
		}
	}
	
	void Bin::run(size_t offs) {
		_pc = _ops.begin() + offs;
		
		static void* op_labels[] = {
			&&op_begin, &&op_end, &&op_funcall, &&op_push
		};

		SNABEL_DISPATCH();
	op_begin:
		env.begin(_pc->as<ops::Begin>().parent);
		_pc++;
		SNABEL_DISPATCH();
	op_end:
		env.end();
		_pc++;
		SNABEL_DISPATCH();
	op_funcall:
		auto &op(_pc->as<ops::Funcall>());
		auto fimp(op.fimp);
		
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
		SNABEL_DISPATCH();
	op_push:
		env.push_stack(_pc->as<ops::Push>().value); 
		_pc++;
		SNABEL_DISPATCH();
	}
}
