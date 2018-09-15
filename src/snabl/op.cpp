#include "snabl/env.hpp"
#include "snabl/fimp.hpp"
#include "snabl/func.hpp"
#include "snabl/lambda.hpp"
#include "snabl/op.hpp"

namespace snabl {
	size_t AOpType::next_label_offs(0);

	OpImp AOpType::make_imp(Env &env, Op &op) const {
		return [&env](Ops::const_iterator end_pc) {
			env.next = (++env.pc == end_pc) ? nullopt : make_optional(env.pc->imp);
		};
	}

	namespace ops {
		const OpType<Call> Call::type("call");
		const OpType<DDrop> DDrop::type("ddrop");
		const Drop::Type Drop::type("drop");
		const Dup::Type Dup::type("dup");
		const Else::Type Else::type("else");
		const Eqval::Type Eqval::type("eqval");
		const Fimp::Type Fimp::type("fimp");
		const OpType<FimpEnd> FimpEnd::type("fimp-end");
		const Funcall::Type Funcall::type("funcall");
		const OpType<Get> Get::type("get");
		const Isa::Type Isa::type("isa");
		const Lambda::Type Lambda::type("lambda");
		const LambdaEnd::Type LambdaEnd::type("lambda-end");
		const OpType<Let> Let::type("let");
		const OpType<Nop> Nop::type("nop");
		const Push::Type Push::type("push");
		const OpType<Recall> Recall::type("recall");
		const OpType<Rot> Rot::type("rot");
		const RSwap::Type RSwap::type("rswap");
		const SDrop::Type SDrop::type("sdrop");
		const Skip::Type Skip::type("skip");
		const OpType<Split> Split::type("split");
		const OpType<SplitEnd> SplitEnd::type("split-end");
		const OpType<Stack> Stack::type("stack");
		const OpType<Swap> Swap::type("swap");
		const Try::Type Try::type("try");
		const TryEnd::Type TryEnd::type("try-end");

		OpImp Drop::Type::make_imp(Env &env, Op &op) const {
			return [&env](Ops::const_iterator end_pc) {
				if (env._stack.size() <= env._stack_offs) { throw Error("Nothing to drop"); }
				env._stack.pop_back();
				env.next = (++env.pc == end_pc) ? nullopt : make_optional(env.pc->imp);
			};
		};

		OpImp Dup::Type::make_imp(Env &env, Op &op) const {
			return [&env](Ops::const_iterator end_pc) {
				if (env._stack.size() <= env._stack_offs) { throw Error("Nothing to dup"); }
				env.push(env._stack.back());
				env.next = (++env.pc == end_pc) ? nullopt : make_optional(env.pc->imp);
			};
		};

		OpImp Else::Type::make_imp(Env &env, Op &op) const {
			return [&env, &op](Ops::const_iterator end_pc) {
				const auto &o(op.as<ops::Else>());
				const auto &v(env.peek());
				if (!v.as<bool>()) { env.pc += *o.nops; }
				env.pop();
				env.next = (++env.pc == end_pc) ? nullopt : make_optional(env.pc->imp);
			};
		};

		void Eqval::Type::dump(const Eqval &op, ostream &out) const {
			if (op.rhs) {
				out << ' ';
				op.rhs->dump(out);
			}
		}

		OpImp Eqval::Type::make_imp(Env &env, Op &op) const {
			return [&env, &op](Ops::const_iterator end_pc) {
				const auto &o(op.as<ops::Eqval>());

				if (env._stack.size() <= env._stack_offs+(o.rhs ? 0 : 1)) {
					throw Error("Nothing to eqval");
				}
				
				const auto lhs(env.pop());
				const auto rhs(o.rhs ? *o.rhs : env.pop());
				env.push(env.bool_type, lhs.eqval(rhs)); 
				env.next = (++env.pc == end_pc) ? nullopt : make_optional(env.pc->imp);
			};
		};
		
		void Fimp::Type::dump(const Fimp &op, ostream &out) const {
			out << ' ' << op.ptr->id << ' ' << op.ptr->nops();
		}

		OpImp Fimp::Type::make_imp(Env &env, Op &op) const {
			return [&env, &op](Ops::const_iterator end_pc) {
				auto &fimp(*op.as<ops::Fimp>().ptr);
				if (fimp.opts() & Target::Opts::Vars) { fimp._parent_scope = env.scope(); }
				env.pc += fimp.nops()+1;
				env.next = (env.pc == end_pc) ? nullopt : make_optional(env.pc->imp);
			};
		};
		
		Funcall::Funcall(const FuncPtr &func): func(func) { }
		Funcall::Funcall(const FimpPtr &fimp): func(fimp->func), fimp(fimp) { }

		void Funcall::Type::dump(const Funcall &op, ostream &out) const {
			out << ' ' << (op.fimp ? op.fimp->id : op.func->id);
			if (op.prev_fimp) { out << " (" << op.prev_fimp->id << ')'; }
		}

		OpImp Funcall::Type::make_imp(Env &env, Op &op) const {
			return [&env, &op](Ops::const_iterator end_pc) {
				auto &o(op.as<ops::Funcall>());
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
				env.next = (env.pc == end_pc) ? nullopt : make_optional(env.pc->imp);
			};
		};

		void Isa::Type::dump(const Isa &op, ostream &out) const {
			out << ' ' << op.rhs->id;
		}

		OpImp Lambda::Type::make_imp(Env &env, Op &op) const {			
			return [&env, &op](Ops::const_iterator end_pc) {
				const auto &o(op.as<ops::Lambda>());

				env.push(env.lambda_type,
								 make_shared<snabl::Lambda>((o.opts & Target::Opts::Vars)
																						? env.scope()
																						: nullptr,
																						*o.start_pc, *o.nops,
																						o.opts));
				env.pc += *o.nops+1;
				env.next = (env.pc == end_pc) ? nullopt : make_optional(env.pc->imp);
			};
		};

		OpImp LambdaEnd::Type::make_imp(Env &env, Op &op) const {
			return [&env](Ops::const_iterator end_pc) {
				const auto &c(env.call());
				const auto &l(*dynamic_pointer_cast<snabl::Lambda>(c.target));
				if (l.opts() & Target::Opts::Vars) { env.end_scope(); }
				env.pc = *c.return_pc;
				env.end_call();

				env.next = (env.pc == end_pc) ? nullopt : make_optional(env.pc->imp);
			};
		};

		void Push::Type::dump(const Push &op, ostream &out) const {
			out << ' ';
			op.val.dump(out);
		}

		OpImp Push::Type::make_imp(Env &env, Op &op) const {
			return [&env, &op](Ops::const_iterator end_pc) {
				env.push(op.as<ops::Push>().val);
				env.next = (++env.pc == end_pc) ? nullopt : make_optional(env.pc->imp);
			};
		};

		OpImp RSwap::Type::make_imp(Env &env, Op &op) const {
			return [&env](Ops::const_iterator end_pc) {
				if (env._stack.size() <= env._stack_offs+2) {
					throw Error("Nothing to rswap");
				}
				
				const auto i(env._stack.size()-1);
				swap(env._stack[i], env._stack[i-2]);
				env.next = (++env.pc == end_pc) ? nullopt : make_optional(env.pc->imp);
			};
		};

		OpImp SDrop::Type::make_imp(Env &env, Op &op) const {
			return [&env, &op](Ops::const_iterator end_pc) {
				if (env._stack.size() <= env._stack_offs+1) {
					throw Error("Nothing to sdrop");
				}
				
				const auto i(env._stack.size()-1);
				env._stack[i-1] = env._stack[i];	
				env._stack.pop_back();
				env.next = (++env.pc == end_pc) ? nullopt : make_optional(env.pc->imp);
			};
		};
		
		OpImp Try::Type::make_imp(Env &env, Op &op) const {			
			return [&env, &op](Ops::const_iterator end_pc) {
				auto &o(op.as<ops::Try>());
				o.state.emplace(env);
				env._tries.push_back(&o);
				env.next = (++env.pc == end_pc) ? nullopt : make_optional(env.pc->imp);
			};
		};

		OpImp TryEnd::Type::make_imp(Env &env, Op &op) const {
			return [&env](Ops::const_iterator end_pc) {
				env._tries.back()->state.reset();
				env._tries.pop_back();
				env.next = (++env.pc == end_pc) ? nullopt : make_optional(env.pc->imp);
			};
		};
	}
}
