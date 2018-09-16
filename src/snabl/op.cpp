#include "snabl/env.hpp"
#include "snabl/fimp.hpp"
#include "snabl/func.hpp"
#include "snabl/lambda.hpp"
#include "snabl/op.hpp"

namespace snabl {
	namespace ops {
		const Call::Type Call::type("call");
		const DDrop::Type DDrop::type("ddrop");
		const Drop::Type Drop::type("drop");
		const Dup::Type Dup::type("dup");
		const Else::Type Else::type("else");
		const Eqval::Type Eqval::type("eqval");
		const Fimp::Type Fimp::type("fimp");
		const FimpEnd::Type FimpEnd::type("fimp-end");
		const Funcall::Type Funcall::type("funcall");
		const Get::Type Get::type("get");
		const Isa::Type Isa::type("isa");
		const Lambda::Type Lambda::type("lambda");
		const LambdaEnd::Type LambdaEnd::type("lambda-end");
		const Let::Type Let::type("let");
		const Nop::Type Nop::type("nop");
		const Push::Type Push::type("push");
		const Recall::Type Recall::type("recall");
		const Rot::Type Rot::type("rot");
		const RSwap::Type RSwap::type("rswap");
		const SDrop::Type SDrop::type("sdrop");
		const Skip::Type Skip::type("skip");
		const Split::Type Split::type("split");
		const SplitEnd::Type SplitEnd::type("split-end");
		const Stack::Type Stack::type("stack");
		const Swap::Type Swap::type("swap");
		const Try::Type Try::type("try");
		const TryEnd::Type TryEnd::type("try-end");

		OpImp Call::Type::make_imp(Env &env, Op &op) const {
			const auto &pos(op.pos);
				
			return [&env, &pos](Ops::const_iterator end_pc) {
				env.pc++;
				env.pop().call(pos, false);
				env.next = (env.pc == end_pc) ? nullptr : &env.pc->imp;
			};
		};

		OpImp DDrop::Type::make_imp(Env &env, Op &op) const {
			return [&env](Ops::const_iterator end_pc) {
				if (env._stack.size() <= env._stack_offs) { throw Error("Nothing to ddrop"); }
				env._stack.pop_back();
				env._stack.pop_back();
				env.next = (++env.pc == end_pc) ? nullptr : &env.pc->imp;
			};
		};

		OpImp Drop::Type::make_imp(Env &env, Op &op) const {
			return [&env, &op](Ops::const_iterator end_pc) {
				if (env._stack.size() <= env._stack_offs) {
					throw RuntimeError(env, op.pos,
														 fmt("Nothing to drop: %0/%1",
																 {env._stack.size(), env._stack_offs}));
				}
				
				env._stack.pop_back();
				env.next = (++env.pc == end_pc) ? nullptr : &env.pc->imp;
			};
		};

		OpImp Dup::Type::make_imp(Env &env, Op &op) const {
			return [&env](Ops::const_iterator end_pc) {
				if (env._stack.size() <= env._stack_offs) { throw Error("Nothing to dup"); }
				env.push(env._stack.back());
				env.next = (++env.pc == end_pc) ? nullptr : &env.pc->imp;
			};
		};

		OpImp Else::Type::make_imp(Env &env, Op &op) const {
			const auto &nops(op.as<ops::Else>().nops);

			return [&env, &nops](Ops::const_iterator end_pc) {
				const auto &v(env.peek());
				if (!v.as<bool>()) { env.pc += *nops; }
				env.pop();
				env.next = (++env.pc == end_pc) ? nullptr : &env.pc->imp;
			};
		};

		void Eqval::Type::dump_data(const Eqval &op, ostream &out) const {
			if (op.rhs) {
				out << ' ';
				op.rhs->dump(out);
			}
		}

		OpImp Eqval::Type::make_imp(Env &env, Op &op) const {
			const auto &o(op.as<ops::Eqval>());
			
			return [&env, &o](Ops::const_iterator end_pc) {
				if (env._stack.size() <= env._stack_offs+(o.rhs ? 0 : 1)) {
					throw Error("Nothing to eqval");
				}
				
				const auto lhs(env.pop());
				const auto rhs(o.rhs ? *o.rhs : env._stack.back());
				env.push(env.bool_type, lhs.eqval(rhs)); 
				env.next = (++env.pc == end_pc) ? nullptr : &env.pc->imp;
			};
		};
		
		void Fimp::Type::dump_data(const Fimp &op, ostream &out) const {
			out << ' ' << op.ptr->id << ' ' << op.ptr->nops();
		}

		OpImp Fimp::Type::make_imp(Env &env, Op &op) const {
			auto &fimp(*op.as<ops::Fimp>().ptr);

			return [&env, &fimp](Ops::const_iterator end_pc) {
				if (fimp.opts() & Target::Opts::Vars) { fimp._parent_scope = env.scope(); }
				env.pc += fimp.nops()+1;
				env.next = (env.pc == end_pc) ? nullptr : &env.pc->imp;
			};
		};
		
		OpImp FimpEnd::Type::make_imp(Env &env, Op &op) const {
			const auto &o(op.as<ops::FimpEnd>());

			return [&env, &o](Ops::const_iterator end_pc) {
				if (o.end_scope) { env.end_scope(); }
				const auto &c(env.call());
				dynamic_pointer_cast<snabl::Fimp>(c.target)->_is_calling = false;				
				env.pc = env.ops.begin()+*c.return_pc;
				env.end_call();
				env.unsplit();
				env.next = (env.pc == end_pc) ? nullptr : &env.pc->imp;
			};
		};

		Funcall::Funcall(const FuncPtr &func): func(func) { }
		Funcall::Funcall(const FimpPtr &fimp): func(fimp->func), fimp(fimp) { }

		void Funcall::Type::dump_data(const Funcall &op, ostream &out) const {
			out << ' ' << (op.fimp ? op.fimp->id : op.func->id);
			if (op.prev_fimp) { out << " (" << op.prev_fimp->id << ')'; }
		}

		OpImp Funcall::Type::make_imp(Env &env, Op &op) const {
			auto &o(op.as<ops::Funcall>());

			return [&env, &o](Ops::const_iterator end_pc) {
				const FimpPtr *fimp(nullptr);

				if (env._stack.size() >= env._stack_offs+o.func->nargs) {
					if (o.fimp) { fimp = &o.fimp; }
					if (!fimp && o.prev_fimp) { fimp = &o.prev_fimp; }

					if (fimp) {
						if (o.func->nargs &&
								!(*fimp)->score(env._stack.begin()+(env._stack.size()-o.func->nargs),
																env._stack.end())) { fimp = nullptr; }
					} else {
						fimp = o.func->get_best_fimp(env._stack.begin()+
																				 (env._stack.size()-o.func->nargs),
																				 env._stack.end());
					}
				}	
			
				if (!fimp) {
					throw RuntimeError(env, env.pc->pos, fmt("Func not applicable: %0",
																									 {o.func->id}));
				}
			
				if (!o.fimp) { o.prev_fimp = *fimp; }
				const auto pos(env.pc->pos);
			
				env.pc++;
				snabl::Fimp::call(*fimp, pos);
				env.next = (env.pc == end_pc) ? nullptr : &env.pc->imp;
			};
		};

		OpImp Get::Type::make_imp(Env &env, Op &op) const {
			const auto &id(op.as<ops::Get>().id);
			
			return [&env, &id](Ops::const_iterator end_pc) {
				auto v(env.scope()->get(id));
				if (!v) { throw Error("Unknown var"); }
				env.push(*v);
				env.next = (++env.pc == end_pc) ? nullptr : &env.pc->imp;
			};
		};

		void Isa::Type::dump_data(const Isa &op, ostream &out) const {
			out << ' ' << op.rhs->id;
		}

		OpImp Isa::Type::make_imp(Env &env, Op &op) const {
			const auto &rhs(op.as<ops::Isa>().rhs);
			
			return [&env, &rhs](Ops::const_iterator end_pc) {
				if (env._stack.size() <= env._stack_offs) { throw Error("Nothing to isa"); }
				const bool ok(env._stack.back().isa(rhs));
				env._stack.pop_back();
				env.push(env.bool_type, ok);
				env.next = (++env.pc == end_pc) ? nullptr : &env.pc->imp;
			};
		};

		OpImp Lambda::Type::make_imp(Env &env, Op &op) const {			
			const auto &o(op.as<ops::Lambda>());
			
			return [&env, &o](Ops::const_iterator end_pc) {
				env.push(env.lambda_type,
								 make_shared<snabl::Lambda>((o.opts & Target::Opts::Vars)
																						? env.scope()
																						: nullptr,
																						*o.start_pc, *o.nops,
																						o.opts));
				env.pc += *o.nops+1;
				env.next = (env.pc == end_pc) ? nullptr : &env.pc->imp;
			};
		};

		OpImp LambdaEnd::Type::make_imp(Env &env, Op &op) const {
			return [&env](Ops::const_iterator end_pc) {
				const auto &c(env.call());
				const auto &l(*dynamic_pointer_cast<snabl::Lambda>(c.target));
				if (l.opts() & Target::Opts::Vars) { env.end_scope(); }
				env.pc = env.ops.begin()+*c.return_pc;
				env.end_call();

				env.next = (env.pc == end_pc) ? nullptr : &env.pc->imp;
			};
		};

		OpImp Let::Type::make_imp(Env &env, Op &op) const {
			const auto &id(op.as<ops::Let>().id);
			
			return [&env, &id](Ops::const_iterator end_pc) {
				if (env._stack.size() <= env._stack_offs) { throw Error("Nothing to let"); }
				auto &v(env._stack.back());
				env.scope()->let(id, v);
				env._stack.pop_back();
				env.next = (++env.pc == end_pc) ? nullptr : &env.pc->imp;
			};
		};

		OpImp Nop::Type::make_imp(Env &env, Op &op) const {
			return [&env](Ops::const_iterator end_pc) {
				env.next = (++env.pc == end_pc) ? nullptr : &env.pc->imp;
			};
		}

		void Push::Type::dump_data(const Push &op, ostream &out) const {
			out << ' ';
			op.val.dump(out);
		}

		OpImp Push::Type::make_imp(Env &env, Op &op) const {
			const auto &v(op.as<ops::Push>().val);
			
			return [&env, &v](Ops::const_iterator end_pc) {
				env.push(v);
				env.next = (++env.pc == end_pc) ? nullptr : &env.pc->imp;
			};
		};

		OpImp Recall::Type::make_imp(Env &env, Op &op) const {
			return [&env](Ops::const_iterator end_pc) {
				env.call().recall();
				env.next = (env.pc == end_pc) ? nullptr : &env.pc->imp;
			};
		};

		OpImp Rot::Type::make_imp(Env &env, Op &op) const {
			return [&env](Ops::const_iterator end_pc) {
				if (env._stack.size() <= env._stack_offs+2) { throw Error("Nothing to rot"); }
				auto i(env._stack.size()-1);
				swap(env._stack[i], env._stack[i-2]);
				swap(env._stack[i], env._stack[i-1]);
				env.next = (++env.pc == end_pc) ? nullptr : &env.pc->imp;
			};
		};

		OpImp RSwap::Type::make_imp(Env &env, Op &op) const {
			return [&env](Ops::const_iterator end_pc) {
				if (env._stack.size() <= env._stack_offs+2) {
					throw Error("Nothing to rswap");
				}
				
				const auto i(env._stack.size()-1);
				swap(env._stack[i], env._stack[i-2]);
				env.next = (++env.pc == end_pc) ? nullptr : &env.pc->imp;
			};
		};

		OpImp SDrop::Type::make_imp(Env &env, Op &op) const {
			return [&env](Ops::const_iterator end_pc) {
				if (env._stack.size() <= env._stack_offs+1) {
					throw Error("Nothing to sdrop");
				}
				
				const auto i(env._stack.size()-1);
				env._stack[i-1] = env._stack[i];	
				env._stack.pop_back();
				env.next = (++env.pc == end_pc) ? nullptr : &env.pc->imp;
			};
		};
		
		OpImp Skip::Type::make_imp(Env &env, Op &op) const {
			const auto &nops(op.as<ops::Skip>().nops);
			
			return [&env, &nops](Ops::const_iterator end_pc) {
				env.pc += *nops+1;
				env.next = (env.pc == end_pc) ? nullptr : &env.pc->imp;
			};
		};

		OpImp Split::Type::make_imp(Env &env, Op &op) const {
			return [&env](Ops::const_iterator end_pc) {
				env.split();
				env.next = (++env.pc == end_pc) ? nullptr : &env.pc->imp;
			};
		};

		OpImp SplitEnd::Type::make_imp(Env &env, Op &op) const {
			return [&env](Ops::const_iterator end_pc) {
				env.unsplit();
				env.next = (++env.pc == end_pc) ? nullptr : &env.pc->imp;
			};
		};

		OpImp Stack::Type::make_imp(Env &env, Op &op) const {
			auto unsplit(op.as<Stack>().unsplit);
			
			return [&env, unsplit](Ops::const_iterator end_pc) {
				const size_t offs(env._stack_offs);
				if (unsplit) { env.unsplit(); }
				auto s(make_shared<snabl::Stack>());
				
				if (env._stack.size() > offs) {
					const auto i(env._stack.begin()+offs), j(env._stack.end());
					move(i, j, back_inserter(*s));
					env._stack.erase(i, j);
				}
				
				env.push(env.stack_type, s);
				env.next = (++env.pc == end_pc) ? nullptr : &env.pc->imp;
			};
		};

		OpImp Swap::Type::make_imp(Env &env, Op &op) const {
			return [&env](Ops::const_iterator end_pc) {
				if (env._stack.size() <= env._stack_offs+1) {
					throw Error("Nothing to swap");
				}
				
				const auto i(env._stack.size()-1);
				swap(env._stack[i], env._stack[i-1]);
				env.next = (++env.pc == end_pc) ? nullptr : &env.pc->imp;
			};
		};
		
		OpImp Try::Type::make_imp(Env &env, Op &op) const {			
			auto &o(op.as<ops::Try>());

			return [&env, &o](Ops::const_iterator end_pc) {
				o.state.emplace(env);
				env._tries.push_back(&o);
				env.next = (++env.pc == end_pc) ? nullptr : &env.pc->imp;
			};
		};

		OpImp TryEnd::Type::make_imp(Env &env, Op &op) const {
			return [&env](Ops::const_iterator end_pc) {
				env._tries.back()->state.reset();
				env._tries.pop_back();
				env.next = (++env.pc == end_pc) ? nullptr : &env.pc->imp;
			};
		};
	}
}
