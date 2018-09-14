#include "snabl/call.hpp"
#include "snabl/env.hpp"
#include "snabl/parser.hpp"
#include "snabl/run.hpp"

#define SNABL_DISPATCH()												\
	if (pc == end_pc) { return; }									\
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

	void Env::run(optional<Ops::iterator> _end_pc) {
		const auto
			start_pc(ops.begin()),
			end_pc(_end_pc ? *_end_pc : ops.end());
		
		static void* op_labels[] = {
			&&op_call, &&op_ddrop, &&op_drop, &&op_dup, &&op_else, &&op_eqval,
			&&op_fimp, &&op_fimp_end, &&op_funcall, &&op_get, &&op_isa, &&op_jump,
			&&op_lambda, &&op_lambda_end, &&op_let, &&op_nop, &&op_push, &&op_recall,
			&&op_rot, &&op_rswap, &&op_sdrop, &&op_skip, &&op_split, &&op_split_end,
			&&op_stack, &&op_swap, &&op_try, &&op_try_end
		};

		try {
			SNABL_DISPATCH();
		op_call: {
				const auto pos(pc->pos);
				pc++;
				pop().call(pos, false);
				SNABL_DISPATCH();
			}
		op_ddrop:
			if (_stack.size() <= _stack_offs) { throw Error("Nothing to ddrop"); }
			_stack.pop_back();
			_stack.pop_back();
			pc++;
			SNABL_DISPATCH();
		op_drop:
			if (_stack.size() <= _stack_offs) { throw Error("Nothing to drop"); }
			_stack.pop_back();
			pc++;
			SNABL_DISPATCH();
		op_dup:
			if (_stack.size() <= _stack_offs) { throw Error("Nothing to dup"); }
			push(_stack.back());
			pc++;
			SNABL_DISPATCH();
		op_else: {
				const auto &op(pc->as<ops::Else>());
				const auto &v(_stack.back());
				if (!v.as<bool>()) { pc += *op.nops; }
				_stack.pop_back();
				pc++;
				SNABL_DISPATCH();
			}
		op_eqval: {
				auto &op(pc->as<ops::Eqval>());

				if (_stack.size() <= _stack_offs+(op.rhs ? 0 : 1)) {
					throw Error("Nothing to eqval");
				}
				
				const auto lhs(pop());
				const auto rhs(op.rhs ? *op.rhs : pop());
				push(bool_type, lhs.eqval(rhs)); 
				pc++;
				SNABL_DISPATCH();
			}
		op_fimp: {
				auto &fimp(*pc->as<ops::Fimp>().ptr);
				if (fimp.opts() & Target::Opts::Vars) { fimp._parent_scope = scope(); }
				pc += pc->as<ops::Fimp>().ptr->nops()+1;
				SNABL_DISPATCH();
			}
		op_fimp_end: {
				const auto &op(pc->as<ops::FimpEnd>());
				if (op.end_scope) { end_scope(); }
				const auto &c(call());
				pc = *c.return_pc;
				end_call();
				unsplit();
				SNABL_DISPATCH();
			}
		op_funcall: {
				auto &op(pc->as<ops::Funcall>());
				const FimpPtr *fimp(nullptr);

				if (_stack.size() >= _stack_offs+op.func->nargs) {
					if (op.fimp) { fimp = &op.fimp; }
					if (!fimp && op.prev_fimp) { fimp = &op.prev_fimp; }

					if (fimp) {
						if (op.func->nargs &&
								!(*fimp)->score(_stack.begin()+(_stack.size()-op.func->nargs),
																_stack.end())) { fimp = nullptr; }
					} else {
						fimp = op.func->get_best_fimp(_stack.begin()+
																					(_stack.size()-op.func->nargs),
																					_stack.end());
					}
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
				if (_stack.size() <= _stack_offs) { throw Error("Nothing to isa"); }
				const auto lhs(pop());
				push(bool_type, lhs.isa(pc->as<ops::Isa>().rhs)); 
				pc++;
				SNABL_DISPATCH();
			}
		op_jump:
			pc = start_pc + *pc->as<ops::Jump>().pc;
			SNABL_DISPATCH();
		op_lambda: {
				const auto &op(pc->as<ops::Lambda>());

				push(lambda_type,
						 make_shared<Lambda>((op.opts & Target::Opts::Vars) ? scope() : nullptr,
																 *op.start_pc, *op.nops,
																 op.opts));
				pc += *op.nops+1;
				SNABL_DISPATCH();
			}
		op_lambda_end: {
				const auto &c(call());
				const auto &l(*dynamic_pointer_cast<Lambda>(c.target));
				if (l.opts() & Target::Opts::Vars) { end_scope(); }
				pc = *c.return_pc;
				end_call();
				SNABL_DISPATCH();
			}
		op_let: {
				if (_stack.size() <= _stack_offs) { throw Error("Nothing to let"); }
				const auto &op(pc->as<ops::Let>());
				auto &v(_stack.back());
				scope()->let(op.id, v);
				_stack.pop_back();
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
				if (_stack.size() <= _stack_offs+2) { throw Error("Nothing to rot"); }
				auto i(_stack.size()-1);
				swap(_stack[i], _stack[i-2]);
				swap(_stack[i], _stack[i-1]);
				pc++;
				SNABL_DISPATCH();
			}
		op_rswap: {
				if (_stack.size() <= _stack_offs+2) { throw Error("Nothing to rswap"); }
				auto i(_stack.size()-1);
				swap(_stack[i], _stack[i-2]);
				pc++;
				SNABL_DISPATCH();
			}
		op_sdrop:
			if (_stack.size() <= _stack_offs+1) { throw Error("Nothing to sdrop"); }
			const auto i(_stack.size()-1);
			_stack[i-1] = _stack[i];	
			_stack.pop_back();
			pc++;
			SNABL_DISPATCH();
		op_skip:
			pc += *pc->as<ops::Skip>().nops+1;
			SNABL_DISPATCH();
		op_split:
			split();
			pc++;
			SNABL_DISPATCH();
		op_split_end:
			unsplit();
			pc++;
			SNABL_DISPATCH();
		op_stack: {
				auto &op(pc->as<ops::Stack>());
				const size_t offs(_stack_offs);
				if (op.unsplit) { unsplit(); }
				auto s(make_shared<Stack>());
				
				if (_stack.size() > offs) {
					auto i(_stack.begin()+offs), j(_stack.end());
					move(i, j, back_inserter(*s));
					_stack.erase(i, j);
				}
				
				push(stack_type, s);
				pc++;
				SNABL_DISPATCH();
			}
		op_swap: {
				if (_stack.size() <= _stack_offs+1) { throw Error("Nothing to swap"); }
				auto i(_stack.size()-1);
				swap(_stack[i], _stack[i-1]);
				pc++;
				SNABL_DISPATCH();
			}
		op_try: {
				auto &op(pc->as<ops::Try>());
				op.state.emplace(*this);
				_tries.push_back(&op);
				pc = start_pc+*op.body_pc;
				SNABL_DISPATCH();
			}
		op_try_end:
			_tries.back()->state.reset();
			_tries.pop_back();
			pc++;
			SNABL_DISPATCH();				
		} catch (const UserError &e) {
			if (_tries.empty()) { throw e; }
			auto &t(*_tries.back());
			_tries.pop_back();
			t.state->restore_all(*this);
			t.state.reset();
			push(error_type, make_shared<UserError>(e));
			pc = start_pc+*t.start_pc;
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
