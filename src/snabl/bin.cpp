#include "snabl/bin.hpp"
#include "snabl/env.hpp"

#define SNABL_DISPATCH()												\
	if (pc == ops.end()) { return; }							\
	goto *op_labels[pc->type.label_offs];					\

namespace snabl {
	Bin::Bin(Env &env): env(env) { }

	void Bin::compile(const Forms::const_iterator &begin,
										const Forms::const_iterator &end) {
		FuncPtr func;
		FimpPtr fimp;

		for (auto i(begin); i != end;) { i->imp->compile(i, end, func, fimp, *this); }
		const auto pos(begin->pos);

		if (fimp) {
			Fimp::compile(fimp, pos);
			emplace_back(ops::Funcall::type, pos, fimp);
		} else if (func) {
			emplace_back(ops::Funcall::type, pos, func);
		}
	}

	void Bin::compile(const Forms &forms) { compile(forms.begin(), forms.end()); }

	void Bin::run(size_t start_pc) {
		pc = ops.begin()+start_pc;
		
		static void* op_labels[] = {
			&&op_begin, &&op_drop, &&op_else, &&op_end, &&op_fimp, &&op_funcall,
			&&op_getvar, &&op_push, &&op_putvar, &&op_return, &&op_skip
		};

		SNABL_DISPATCH();
	op_begin:
		env.begin();
		pc++;
		SNABL_DISPATCH();
	op_drop:
		env.pop_stack();
		pc++;
		SNABL_DISPATCH();
	op_else: {
			const auto &op(pc->as<ops::Else>());
			const auto v(env.pop_stack());
			if (!v.as<bool>()) { pc += op.nops; }
			pc++;
			SNABL_DISPATCH();
		}
	op_end:
		env.end();
		pc++;
		SNABL_DISPATCH();
	op_fimp:
		pc++;
		SNABL_DISPATCH();		
	op_funcall: {
			auto &op(pc->as<ops::Funcall>());
			FimpPtr fimp(op.fimp);
			
			if (!fimp && op.prev_fimp) { fimp = op.prev_fimp; }
			
			if (fimp) {
				if (!fimp->score(env.stack())) { fimp = nullptr; }
			} else {
				fimp = op.func->get_best_fimp(env.stack());
			}
			
			if (!fimp) { throw Error(fmt("Func not applicable: %0", {fimp->func->id})); }
			if (!op.fimp) { op.prev_fimp = fimp; }
			if (Fimp::call(fimp, pc->pos)) { pc++; }
			SNABL_DISPATCH();
		}
	op_getvar: {
			const auto &op(pc->as<ops::GetVar>());		
			auto v(env.get_var(op.id));
			if (!v) { throw Error("Unknown var"); }
			env.push_stack(*v);
			pc++;
			SNABL_DISPATCH();
		}
	op_push:
		env.push_stack(pc->as<ops::Push>().value); 
		pc++;
		SNABL_DISPATCH();
	op_putvar: {
			const auto &op(pc->as<ops::PutVar>());
			auto v(env.pop_stack());
			env.scope()->put_var(op.id, v);
			pc++;
			SNABL_DISPATCH();
		}
	op_return: {
			const auto fn(pc->as<ops::Return>().fimp->func);
			const auto call(env.pop_call());
			env.end();
			if (env.stack().size() < fn->nrets) { throw Error("Nothing to return"); }
			pc = *call.return_pc;
			SNABL_DISPATCH();
		}
	op_skip:
		pc += pc->as<ops::Skip>().nops+1;
		SNABL_DISPATCH();
	}
}
