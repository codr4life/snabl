#include "snabl/call.hpp"
#include "snabl/env.hpp"
#include "snabl/parser.hpp"
#include "snabl/run.hpp"

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
			&&op_call, &&op_ddrop, &&op_drop, &&op_dup, &&op_else, &&op_eqval,
			&&op_fimp, &&op_fimpret, &&op_funcall, &&op_get, &&op_isa, &&op_lambda,
			&&op_lambdaret, &&op_let, &&op_nop, &&op_push, &&op_recall, &&op_rot,
			&&op_rswap, &&op_sdrop, &&op_skip, &&op_swap, &&op_try
		};

		SNABL_DISPATCH();
	op_call: {
			const auto pos(pc->pos);
			pc++;
			pop().call(pos, false);
			SNABL_DISPATCH();
		}
	op_ddrop:
		if (_stack.size() < 2) { throw Error("Nothing to ddrop"); }
		_stack.pop_back();
		_stack.pop_back();
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
			if (!v.as<bool>()) { pc += *op.nops; }
			pc++;
			SNABL_DISPATCH();
		}
	op_eqval: {
			const auto lhs(pop());
			push(bool_type, lhs.eqval(pc->as<ops::Eqval>().rhs)); 
			pc++;
			SNABL_DISPATCH();
		}
	op_fimp:
		pc += pc->as<ops::Fimp>().ptr->nops()+1;
		SNABL_DISPATCH();
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
				if (!(*fimp)->score(_stack)) { fimp = nullptr; }
			} else {
				fimp = op.func->get_best_fimp(_stack);
			}
			
			if (!fimp) {
				throw RuntimeError(*this, pc->pos, fmt("Func not applicable: %0",
																							 {op.func->id}));
			}
			
			if (!op.fimp) { op.prev_fimp = *fimp; }
			const auto pos(pc->pos);
			
			pc++;
			Fimp::call(*fimp, pos);
			SNABL_DISPATCH();
		}
	op_get: {
			const auto &op(pc->as<ops::Get>());		
			auto v(scope()->get(op.id));
			if (!v) { throw Error("Unknown var"); }
			push(*v);
			pc++;
			SNABL_DISPATCH();
		}
	op_isa: {
			const auto lhs(pop());
			push(bool_type, lhs.isa(pc->as<ops::Isa>().rhs)); 
			pc++;
			SNABL_DISPATCH();
		}
	op_lambda: {
			const auto &op(pc->as<ops::Lambda>());
			push(lambda_type, LambdaPtr::make(op.has_vars ? scope() : nullptr,
																				*op.start_pc, *op.nops,
																				op.has_vars));
			pc += *op.nops+1;
			SNABL_DISPATCH();
		}
	op_lambdaret: {
			const auto &c(call());
			if (c.lambda->has_vars) { end_scope(); }
			pc = *c.return_pc;
			end_call();
			SNABL_DISPATCH();
		}
	op_let: {
			const auto &op(pc->as<ops::Let>());
			auto v(pop());
			scope()->let(op.id, v);
			pc++;
			SNABL_DISPATCH();
		}
	op_nop:
		pc++;
		SNABL_DISPATCH();
	op_push:
		push(pc->as<ops::Push>().val); 
		pc++;
		SNABL_DISPATCH();
	op_recall:
		call().recall();
		SNABL_DISPATCH();
	op_rot: {
			if (_stack.size() < 3) { throw Error("Nothing to rot"); }
			auto i(_stack.size()-1);
			swap(_stack[i], _stack[i-2]);
			swap(_stack[i], _stack[i-1]);
			pc++;
			SNABL_DISPATCH();
		}
	op_rswap: {
			if (_stack.size() < 3) { throw Error("Nothing to rswap"); }
			auto i(_stack.size()-1);
			swap(_stack[i], _stack[i-2]);
			pc++;
			SNABL_DISPATCH();
		}
	op_sdrop:
		if (_stack.size() < 2) { throw Error("Nothing to sdrop"); }
		auto i(_stack.size()-1);
		_stack[i-1] = _stack[i];	
		_stack.pop_back();
		pc++;
		SNABL_DISPATCH();
	op_skip:
		pc += *pc->as<ops::Skip>().nops+1;
		SNABL_DISPATCH();
	op_swap: {
			if (_stack.size() < 2) { throw Error("Nothing to swap"); }
			auto i(_stack.size()-1);
			swap(_stack[i], _stack[i-1]);
			pc++;
			SNABL_DISPATCH();
		}
	op_try: {
			const auto &op(pc->as<ops::Try>());
			const State state(*this);
			
			try {
				pc = ops.begin()+*op.body_pc;
				run(pc+*op.body_nops);
				if (op.push) { push(nil_type); }
			} catch (const UserError &e) {
				state.restore_libs(*this);
				state.restore_scopes(*this);
				state.restore_calls(*this);
				state.restore_stack(*this);
				if (op.push) { push(error_type, ErrorPtr::make(e)); }
			}

			if (*op.body_pc > *op.start_pc) {
				pc = ops.begin()+*op.start_pc;
				run(ops.begin()+*op.body_pc);
			}
			
			pc = ops.begin()+*op.body_pc+*op.body_nops;
			SNABL_DISPATCH();
		}
	}

	RuntimeError::RuntimeError(Env &env, Pos pos, const string &msg) {
		stringstream buf;
				
		buf << env._stack << endl
				<< "Error in row " << pos.row << ", col " << pos.col << ":\n"
				<< msg;
		
		_what = buf.str();
	}

	const char *RuntimeError::what() const noexcept { return _what.c_str(); }

	static string val_str(const Box &val) {
		stringstream buf;
		buf << val;
		return buf.str();
	}
	
	UserError::UserError(Env &env, Pos pos, const Box &_val):
		RuntimeError(env, pos, val_str(_val)), val(_val) { }
}
