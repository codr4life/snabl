#include "snabl/env.hpp"

#define SNABL_DISPATCH()												\
	if (pc == *end_pc) { return; }								\
	goto *op_labels[pc->type.label_offs];					\

namespace snabl {
	void Env::run(string_view in) {
		auto offs(ops.size());
		compile(in);
		pc = ops.begin()+offs;		
		run();
	}

	void Env::run(istream &in) {
		Forms fs;
		parse(in, fs);
		auto offs(ops.size());
		compile(fs.begin(), fs.end());
		pc = ops.begin()+offs;		
		run();
	}

	void Env::run(optional<PC> end_pc) {
		if (!end_pc) { end_pc = ops.end(); }
		
		static void* op_labels[] = {
			&&op_begin, &&op_call, &&op_drop, &&op_dup, &&op_else, &&op_end, &&op_fimp,
			&&op_fimpret, &&op_funcall, &&op_getvar, &&op_lambda, &&op_lambdaret,
			&&op_push, &&op_putvar, &&op_skip, &&op_swap
		};

		SNABL_DISPATCH();
	op_begin:
		begin_scope(pc->as<ops::Begin>().parent);
		pc++;
		SNABL_DISPATCH();
	op_call:
		pc++;
		pop().call(false);
		SNABL_DISPATCH();
	op_drop:
		pop();
		pc++;
		SNABL_DISPATCH();
	op_dup:
		push(_stack.back());
		pc++;
		SNABL_DISPATCH();
	op_else: {
			const auto &op(pc->as<ops::Else>());
			const auto v(pop());
			if (!v.as<bool>()) { pc += op.nops; }
			pc++;
			SNABL_DISPATCH();
		}
	op_end:
		end_scope();
		pc++;
		SNABL_DISPATCH();
	op_fimp:
		pc++;
		SNABL_DISPATCH();		
	op_fimpret: {
			const auto &c(call());
			const auto fn(c.target<FimpPtr>()->func);
			end_scope();
			auto stack_offs(end_stack());

			if (_stack.size() != stack_offs+fn->nrets) {
				throw Error("Invalid return stack");
			}
			
			if (!c.return_pc) { throw Error("Missing return pc"); }
			
			end_call();
			pc = *c.return_pc;
			SNABL_DISPATCH();
		}
	op_funcall: {
			auto &op(pc->as<ops::Funcall>());
			FimpPtr fimp(op.fimp);
			
			if (!fimp && op.prev_fimp) { fimp = op.prev_fimp; }
			
			if (fimp) {
				if (!fimp->score(_stack)) { fimp = nullptr; }
			} else {
				fimp = op.func->get_best_fimp(_stack);
			}
			
			if (!fimp) { throw Error(fmt("Func not applicable: %0", {op.func->id})); }
			if (!op.fimp) { op.prev_fimp = fimp; }
			
			pc++;
			Fimp::call(fimp, pc->pos);
			SNABL_DISPATCH();
		}
	op_getvar: {
			const auto &op(pc->as<ops::GetVar>());		
			auto v(scope()->get_var(op.id));
			if (!v) { throw Error("Unknown var"); }
			push(*v);
			pc++;
			SNABL_DISPATCH();
		}
	op_lambda: {
			const auto &op(pc->as<ops::Lambda>());
			push(lambda_type, scope(), *op.start_pc, op.nops);
			pc += op.nops+1;
			SNABL_DISPATCH();
		}
	op_lambdaret: {
			const auto &c(call());
			const auto l(c.target<Lambda>());
			end_scope();
			
			if (!c.return_pc) { throw Error("Missing return pc"); }
			
			end_call();
			pc = *c.return_pc;
			SNABL_DISPATCH();
		}
	op_push:
		push(pc->as<ops::Push>().val); 
		pc++;
		SNABL_DISPATCH();
	op_putvar: {
			const auto &op(pc->as<ops::PutVar>());
			auto v(pop());
			scope()->put_var(op.id, v);
			pc++;
			SNABL_DISPATCH();
		}
	op_skip:
		pc += pc->as<ops::Skip>().nops+1;
		SNABL_DISPATCH();
	op_swap:
		auto i(_stack.rbegin());
		iter_swap(i, i+1);
		pc++;
		SNABL_DISPATCH();
	}
}
