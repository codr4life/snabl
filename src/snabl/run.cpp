#include "snabl/call.hpp"
#include "snabl/env.hpp"
#include "snabl/parser.hpp"

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
		Parser(*this).parse(in, fs);
		auto offs(ops.size());
		compile(fs.begin(), fs.end());
		pc = ops.begin()+offs;		
		run();
	}

	void Env::run(optional<PC> end_pc) {
		if (!end_pc) { end_pc = ops.end(); }
		
		static void* op_labels[] = {
			&&op_call, &&op_ddrop, &&op_drop, &&op_dup, &&op_else,
			&&op_fimpret, &&op_funcall, &&op_getvar, &&op_lambda, &&op_lambdaret,
			&&op_push, &&op_putvar, &&op_recall, &&op_rot, &&op_rswap, &&op_sdrop,
			&&op_skip, &&op_swap
		};

		SNABL_DISPATCH();
	op_call:
		pc++;
		pop().call(false);
		SNABL_DISPATCH();
	op_ddrop:
		if (_stack.size() < 2) { throw Error("Nothing to ddrop"); }
		_stack.erase(_stack.begin()+_stack.size()-2, _stack.end());
		pc++;
		SNABL_DISPATCH();
	op_drop:
		if (_stack.empty()) { throw Error("Nothing to drop"); }
		_stack.pop_back();
		pc++;
		SNABL_DISPATCH();
	op_dup:
		if (_stack.empty()) { throw Error("Nothing to dup"); }
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
	op_fimpret: {
			const auto &op(pc->as<ops::FimpRet>());
			if (op.end_scope) { end_scope(); }

			const auto &c(call());
			pc = *c.return_pc;
			end_call();
			SNABL_DISPATCH();
		}
	op_funcall: {
			auto &op(pc->as<ops::Funcall>());
			const FimpPtr *fimp(op.fimp ? &op.fimp : nullptr);
			
			if (!fimp && op.prev_fimp) { fimp = &op.prev_fimp; }
			
			if (fimp) {
				if (_is_safe &&
						!(*fimp)->score(_stack)) {
					fimp = nullptr;
				}
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
			push(lambda_type, LambdaPtr::make(op.has_vars ? scope() : nullptr,
																				*op.start_pc, op.nops,
																				op.has_vars));
			pc += op.nops+1;
			SNABL_DISPATCH();
		}
	op_lambdaret: {
			const auto &c(call());
			if (c.lambda->has_vars) { end_scope(); }
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
			if (_stack.size() < 3) { throw Error("Nothing to rot"); }
			auto i(_stack.rbegin());
			iter_swap(i, i+2);
			iter_swap(i, i+1);
			pc++;
			SNABL_DISPATCH();
		}
	op_rswap: {
			if (_stack.size() < 3) { throw Error("Nothing to rswap"); }
			auto i(_stack.rbegin());
			iter_swap(i, i+2);
			pc++;
			SNABL_DISPATCH();
		}
	op_sdrop:
		if (_stack.size() < 2) { throw Error("Nothing to sdrop"); }
		_stack.erase(_stack.begin()+_stack.size()-2);
		pc++;
		SNABL_DISPATCH();
	op_skip:
		pc += pc->as<ops::Skip>().nops+1;
		SNABL_DISPATCH();
	op_swap:
		if (_stack.size() < 2) { throw Error("Nothing to swap"); }
		auto i(_stack.rbegin());
		iter_swap(i, i+1);
		pc++;
		SNABL_DISPATCH();
	}
}
