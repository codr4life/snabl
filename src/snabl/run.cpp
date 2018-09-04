#include "snabl/call.hpp"
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
			&&op_begin, &&op_call, &&op_ddrop, &&op_drop, &&op_dup, &&op_else, &&op_end,
			&&op_fimpret, &&op_funcall, &&op_getvar, &&op_lambda, &&op_lambdaret,
			&&op_push, &&op_putvar, &&op_recall, &&op_rot, &&op_rswap, &&op_sdrop,
			&&op_skip, &&op_swap
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
	op_ddrop:
		if (_stack.size() < 2) { throw Error("Missing value"); }
		_stack.erase(_stack.begin()+_stack.size()-2, _stack.end());
		pc++;
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
	op_fimpret: {
			const auto &op(pc->as<ops::FimpRet>());
			if (op.end_scope) { end_scope(); }

			const auto &c(call());
			const auto &fn(c.fimp->func);
			auto stack_offs(end_stack());

			if (_is_safe && _stack.size() != stack_offs+fn->nrets) {
				throw Error(fmt("Invalid return stack: %0", {c.fimp->id}));
			}
			
			pc = *c.return_pc;
			end_call();
			SNABL_DISPATCH();
		}
	op_funcall: {
			auto &op(pc->as<ops::Funcall>());
			const FimpPtr *fimp(op.fimp ? &op.fimp : nullptr);
			
			if (!fimp && op.prev_fimp) { fimp = &op.prev_fimp; }
			
			if (fimp) {
				if (_is_safe && !(*fimp)->score(_stack)) { fimp = nullptr; }
			} else {
				fimp = &op.func->get_best_fimp(_stack);
			}
			
			if (!fimp) { throw Error(fmt("Func not applicable: %0", {op.func->id})); }
			if (!op.fimp) { op.prev_fimp = *fimp; }
			
			pc++;
			Fimp::call(*fimp, pc->pos);
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
			push(lambda_type, LambdaPtr::make(scope(), *op.start_pc, op.nops));
			pc += op.nops+1;
			SNABL_DISPATCH();
		}
	op_lambdaret: {
			const auto &c(call());
			end_scope();
			pc = *c.return_pc;
			end_call();
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
	op_recall:
		call().recall();
		SNABL_DISPATCH();
	op_rot: {
			if (_stack.size() < 3) { throw Error("Missing value"); }
			auto i(_stack.rbegin());
			iter_swap(i, i+2);
			iter_swap(i, i+1);
			pc++;
			SNABL_DISPATCH();
		}
	op_rswap: {
			if (_stack.size() < 3) { throw Error("Missing value"); }
			auto i(_stack.rbegin());
			iter_swap(i, i+2);
			pc++;
			SNABL_DISPATCH();
		}
	op_sdrop:
		if (_stack.size() < 2) { throw Error("Missing value"); }
		_stack.erase(_stack.begin()+_stack.size()-2);
		pc++;
		SNABL_DISPATCH();
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
