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
		const Funcall::Type Funcall::type("funcall");
		const Get::Type Get::type("get");
		const Isa::Type Isa::type("isa");
		const Lambda::Type Lambda::type("lambda");
		const Let::Type Let::type("let");
		const Nop::Type Nop::type("nop");
		const Push::Type Push::type("push");
		const Recall::Type Recall::type("recall");
		const Return::Type Return::type("return");
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
			return [&env, &op]() {
				env.pc = op.next;
				env.pop().call(op.pos, false);
			};
		};

		OpImp DDrop::Type::make_imp(Env &env, Op &op) const {
			return [&env, &op]() {
				if (env._stack.size() <= env._stack_offs) { throw Error("Nothing to ddrop"); }
				env._stack.pop_back();
				env._stack.pop_back();
				env.pc = op.next;
			};
		};

		OpImp Drop::Type::make_imp(Env &env, Op &op) const {
			return [&env, &op]() {
				if (env._stack.size() <= env._stack_offs) {
					throw RuntimeError(env, op.pos,
														 fmt("Nothing to drop: %0/%1",
																 {env._stack.size(), env._stack_offs}));
				}
				
				env._stack.pop_back();
				env.pc = op.next;
			};
		};

		OpImp Dup::Type::make_imp(Env &env, Op &op) const {
			return [&env, &op]() {
				if (env._stack.size() <= env._stack_offs) { throw Error("Nothing to dup"); }
				env.push(env._stack.back());
				env.pc = op.next;
			};
		};

		OpImp Else::Type::make_imp(Env &env, Op &op) const {
			const auto skip_pc(&op.as<ops::Else>().skip_pc);

			return [&env, &op, skip_pc]() {
				const auto &v(env.peek());

				if (v.type() != env.bool_type) {
					throw RuntimeError(env, op.pos, fmt("Invalid else cond: %0", {v}));
				}
				
				env.pc = v.as<bool>() ? op.next : skip_pc;
				env.pop();
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
			
			return [&env, &op, &o]() {
				if (env._stack.size() <= env._stack_offs+(o.rhs ? 0 : 1)) {
					throw Error("Nothing to eqval");
				}
				
				const auto lhs(env.pop());
				const auto rhs(o.rhs ? *o.rhs : env._stack.back());
				env.push(env.bool_type, lhs.eqval(rhs));
				env.pc = op.next;
			};
		};
		
		void Fimp::Type::dump_data(const Fimp &op, ostream &out) const {
			out << ' ' << op.ptr->id;
		}

		OpImp Fimp::Type::make_imp(Env &env, Op &op) const {
			auto &fimp(*op.as<ops::Fimp>().ptr);

			return [&env, &fimp]() {
				if (fimp._opts & Target::Opts::Vars) { fimp._parent_scope = env.scope(); }
				env.pc = &fimp._end_pc;
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

			return [&env, &op, &o]() {
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
					throw RuntimeError(env, op.pos, fmt("Func not applicable: %0",
																							{o.func->id}));
				}
			
				if (!o.fimp) { o.prev_fimp = *fimp; }
				env.pc = op.next;
				snabl::Fimp::call(*fimp, op.pos);
			};
		};

		OpImp Get::Type::make_imp(Env &env, Op &op) const {
			const auto &id(op.as<ops::Get>().id);
			
			return [&env, &op, &id]() {
				auto v(env.scope()->get(id));
				if (!v) { throw RuntimeError(env, op.pos, fmt("Unknown var: %0", {id})); }
				env.push(*v);
				env.pc = op.next;
			};
		};

		void Isa::Type::dump_data(const Isa &op, ostream &out) const {
			out << ' ' << op.rhs->id;
		}

		OpImp Isa::Type::make_imp(Env &env, Op &op) const {
			const auto &rhs(op.as<ops::Isa>().rhs);
			
			return [&env, &op, &rhs]() {
				if (env._stack.size() <= env._stack_offs) { throw Error("Nothing to isa"); }
				const bool ok(env._stack.back().isa(rhs));
				env._stack.pop_back();
				env.push(env.bool_type, ok);
				env.pc = op.next;
			};
		};

		OpImp Lambda::Type::make_imp(Env &env, Op &op) const {			
			auto &o(op.as<ops::Lambda>());
			
			return [&env, &o]() {
				env.push(env.lambda_type,
								 make_shared<snabl::Lambda>((o.opts & Target::Opts::Vars)
																						? env.scope()
																						: nullptr,
																						o.start_pc, o.end_pc,
																						o.opts));
				env.pc = &o.end_pc;
			};
		};

		OpImp Let::Type::make_imp(Env &env, Op &op) const {
			const auto &id(op.as<ops::Let>().id);
			
			return [&env, &op, &id]() {
				if (env._stack.size() <= env._stack_offs) { throw Error("Nothing to let"); }
				auto &v(env._stack.back());
				env.scope()->let(id, v);
				env._stack.pop_back();
				env.pc = op.next;
			};
		};

		OpImp Nop::Type::make_imp(Env &env, Op &op) const {
			return [&env, &op]() { env.pc = op.next; };
		}

		void Push::Type::dump_data(const Push &op, ostream &out) const {
			out << ' ';
			op.val.dump(out);
		}

		OpImp Push::Type::make_imp(Env &env, Op &op) const {
			const auto &v(op.as<ops::Push>().val);
			
			return [&env, &op, &v]() {
				env.push(v);
				env.pc = op.next;
			};
		};

		OpImp Recall::Type::make_imp(Env &env, Op &op) const {
			return [&env, &op]() {
				if (!env._target) { throw RuntimeError(env, op.pos, "Nothing to recall"); }
				env._target->recall(env);
			};
		};

		OpImp Return::Type::make_imp(Env &env, Op &op) const {
			const auto &o(op.as<ops::Return>());
			
			return [&env, &op, &o]() {
				if (!env._target) {
					throw RuntimeError(env, op.pos, "Nothing to return from");
				}
				
				auto &t(*env._target);
				if (t.opts() & Target::Opts::Vars) { env.end_scope(); }
				env.pc = t.call()->return_pc;
				if (dynamic_pointer_cast<FimpPtr>(env._target)) { env.unsplit(); }
				t.end_call(env);
			};
		};

		OpImp Rot::Type::make_imp(Env &env, Op &op) const {
			return [&env, &op]() {
				if (env._stack.size() <= env._stack_offs+2) { throw Error("Nothing to rot"); }
				auto i(env._stack.size()-1);
				swap(env._stack[i], env._stack[i-2]);
				swap(env._stack[i], env._stack[i-1]);
				env.pc = op.next;
			};
		};

		OpImp RSwap::Type::make_imp(Env &env, Op &op) const {
			return [&env, &op]() {
				if (env._stack.size() <= env._stack_offs+2) {
					throw Error("Nothing to rswap");
				}
				
				const auto i(env._stack.size()-1);
				swap(env._stack[i], env._stack[i-2]);
				env.pc = op.next;
			};
		};

		OpImp SDrop::Type::make_imp(Env &env, Op &op) const {
			return [&env, &op]() {
				if (env._stack.size() <= env._stack_offs+1) {
					throw Error("Nothing to sdrop");
				}
				
				const auto i(env._stack.size()-1);
				env._stack[i-1] = env._stack[i];	
				env._stack.pop_back();
				env.pc = op.next;
			};
		};
		
		OpImp Skip::Type::make_imp(Env &env, Op &op) const {
			const auto end_pc(&op.as<ops::Skip>().end_pc);
			return [&env, end_pc]() { env.pc = end_pc; };
		};

		OpImp Split::Type::make_imp(Env &env, Op &op) const {
			return [&env, &op]() {
				env.split();
				env.pc = op.next;
			};
		};

		OpImp SplitEnd::Type::make_imp(Env &env, Op &op) const {
			return [&env, &op]() {
				env.unsplit();
				env.pc = op.next;
			};
		};

		OpImp Stack::Type::make_imp(Env &env, Op &op) const {
			auto unsplit(op.as<Stack>().unsplit);
			
			return [&env, &op, unsplit]() {
				const size_t offs(env._stack_offs);
				if (unsplit) { env.unsplit(); }
				auto s(make_shared<snabl::Stack>());
				
				if (env._stack.size() > offs) {
					const auto i(env._stack.begin()+offs), j(env._stack.end());
					move(i, j, back_inserter(*s));
					env._stack.erase(i, j);
				}
				
				env.push(env.stack_type, s);
				env.pc = op.next;
			};
		};

		OpImp Swap::Type::make_imp(Env &env, Op &op) const {
			return [&env, &op]() {
				if (env._stack.size() <= env._stack_offs+1) {
					throw Error("Nothing to swap");
				}
				
				const auto i(env._stack.size()-1);
				swap(env._stack[i], env._stack[i-1]);
				env.pc = op.next;
			};
		};
		
		OpImp Try::Type::make_imp(Env &env, Op &op) const {			
			auto &o(op.as<ops::Try>());

			return [&env, &op, &o]() {
				o.state.emplace(env);
				env._tries.push_back(&o);
				env.pc = op.next;
			};
		};

		OpImp TryEnd::Type::make_imp(Env &env, Op &op) const {
			return [&env, &op]() {
				env._tries.back()->state.reset();
				env._tries.pop_back();
				env.pc = op.next;
			};
		};
	}
}
