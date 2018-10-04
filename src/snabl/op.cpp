#include "snabl/env.hpp"
#include "snabl/fimp.hpp"
#include "snabl/func.hpp"
#include "snabl/lambda.hpp"
#include "snabl/op.hpp"
#include "snabl/timer.hpp"

namespace snabl {
	namespace ops {
		const Bench::Type Bench::type("bench");
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
		const Jump::Type Jump::type("jump");
		const JumpIf::Type JumpIf::type("jump-if");
		const Lambda::Type Lambda::type("lambda");
		const Let::Type Let::type("let");
		const Nop::Type Nop::type("nop");
		const Push::Type Push::type("push");
		const Recall::Type Recall::type("recall");
		const Return::Type Return::type("return");
		const Rot::Type Rot::type("rot");
		const RSwap::Type RSwap::type("rswap");
		const Scope::Type Scope::type("scope");
		const ScopeEnd::Type ScopeEnd::type("scope-end");
		const SDrop::Type SDrop::type("sdrop");
		const Split::Type Split::type("split");
		const SplitEnd::Type SplitEnd::type("split-end");
		const Stack::Type Stack::type("stack");
		const Stop::Type Stop::type("stop");
		const Swap::Type Swap::type("swap");
		const Task::Type Task::type("task");
		const Times::Type Times::type("times");
		const Try::Type Try::type("try");
		const TryEnd::Type TryEnd::type("try-end");
		const Yield::Type Yield::type("yield");

		OpImp Bench::Type::make_imp(Env &env, Op &op) const {
			auto &end_pc(op.as<Bench>().end_pc);
			
			return [&env, &op, &end_pc]() {
				const Int reps(env.pop().as<Int>());

				for (int i(0); i < reps; i++) {
					env.jump(op.next);
					env.run();
				}

				Timer t;

				for (int i(0); i < reps; i++) {
					env.jump(op.next);
					env.run();
				}

				env.push(env.time_type, t.ns());
				env.jump(end_pc);
			};
		}

		OpImp Call::Type::make_imp(Env &env, Op &op) const {
			return [&env, &op]() {
				env.jump(op.next);
				env.pop().call(op.pos);
			};
		}

		OpImp DDrop::Type::make_imp(Env &env, Op &op) const {
			return [&env, &op]() {
				auto &t(*env._task);
				auto &s(t._stack);
				
				if (Int(s.size()) <= t._stack_offs) {
					throw Error("Nothing to ddrop");
				}
				
				s.pop_back();
				s.pop_back();
				env.jump(op.next);
			};
		}

		OpImp Drop::Type::make_imp(Env &env, Op &op) const {
			return [&env, &op]() {
				auto &t(*env._task);
				auto &s(t._stack);

				if (Int(s.size()) <= t._stack_offs) {
					throw RuntimeError(env, op.pos,
														 fmt("Nothing to drop: %0/%1",
																 {s.size(), t._stack_offs}));
				}
				
				s.pop_back();
				env.jump(op.next);
			};
		}

		OpImp Dup::Type::make_imp(Env &env, Op &op) const {
			return [&env, &op]() {
				auto &t(*env._task);
				auto &s(t._stack);
				if (Int(s.size()) <= t._stack_offs) { throw Error("Nothing to dup"); }
				env.push(s.back());
				env.jump(op.next);
			};
		}

		OpImp Else::Type::make_imp(Env &env, Op &op) const {
			const auto &skip_pc(op.as<ops::Else>().skip_pc);

			return [&env, &op, &skip_pc]() {
				const auto &v(env.peek());

				if (v.type() != env.bool_type) {
					throw RuntimeError(env, op.pos, fmt("Invalid else cond: %0", {v}));
				}

				if (v.as<bool>()) {
					env.jump(op.next);
				} else {
					env.jump(skip_pc);
				}

				env.pop();
			};
		}

		void Eqval::Type::dump_data(const Eqval &op, ostream &out) const {
			if (op.rhs) {
				out << ' ';
				op.rhs->dump(out);
			}
		}

		OpImp Eqval::Type::make_imp(Env &env, Op &op) const {
			const auto &o(op.as<ops::Eqval>());
			
			return [&env, &op, &o]() {
				auto &t(*env._task);
				auto &s(t._stack);

				if (Int(s.size()) <= t._stack_offs+(o.rhs ? 0 : 1)) {
					throw Error("Nothing to eqval");
				}
				
				const auto lhs(env.pop());
				const auto rhs(o.rhs ? *o.rhs : s.back());
				env.push(env.bool_type, lhs.eqval(rhs));
				env.jump(op.next);
			};
		}
		
		void Fimp::Type::dump_data(const Fimp &op, ostream &out) const {
			out << ' ' << op.ptr->id;
		}

		OpImp Fimp::Type::make_imp(Env &env, Op &op) const {
			auto &o(op.as<ops::Fimp>());
			auto &fimp(*o.ptr);
			
			return [&env, &o, &fimp]() {
				if (o.is_scope) { fimp._parent_scope = env.scope(); }
				env.jump(fimp._end_pc);
			};
		}
		
		Funcall::Funcall(const FuncPtr &func): func(func) { }
		Funcall::Funcall(const FimpPtr &fimp): func(fimp->func), fimp(fimp) { }

		void Funcall::Type::dump_data(const Funcall &op, ostream &out) const {
			out << ' ' << (op.fimp ? op.fimp->id : op.func->id);
			if (op.prev_fimp) { out << " (" << op.prev_fimp->id << ')'; }
		}

		OpImp Funcall::Type::make_imp(Env &env, Op &op) const {
			auto &o(op.as<ops::Funcall>());

			return [&env, &op, &o]() {
				auto &t(*env._task);
				auto &s(t._stack);

				const FimpPtr *fimp(nullptr);

				if (Int(s.size()) >= t._stack_offs+o.func->nargs) {
					if (o.fimp) { fimp = &o.fimp; }
					if (!fimp && o.prev_fimp) { fimp = &o.prev_fimp; }

					if (fimp) {
						if (o.func->nargs &&
								(*fimp)->score(s.begin()+(s.size()-o.func->nargs),
															 s.end()) == -1) { fimp = nullptr; }
					} else {
						fimp = o.func->get_best_fimp(s.begin()+(s.size()-o.func->nargs),
																				 s.end());
					}
				}	
			
				if (!fimp) {
					throw RuntimeError(env, op.pos, fmt("Func not applicable: %0",
																							{o.func->id}));
				}
			
				if (!o.fimp) { o.prev_fimp = *fimp; }
				env.jump(op.next);
				snabl::Fimp::call(*fimp, op.pos);
			};
		}

		OpImp Get::Type::make_imp(Env &env, Op &op) const {
			const auto &id(op.as<ops::Get>().id);
			
			return [&env, &op, &id]() {
				auto v(env.scope()->get(id));
				if (!v) { throw RuntimeError(env, op.pos, fmt("Unknown var: %0", {id})); }
				env.push(*v);
				env.jump(op.next);
			};
		}

		void Isa::Type::dump_data(const Isa &op, ostream &out) const {
			out << ' ' << op.rhs->id;
		}

		OpImp Isa::Type::make_imp(Env &env, Op &op) const {
			const auto &rhs(op.as<ops::Isa>().rhs);
			
			return [&env, &op, &rhs]() {
				auto &t(*env._task);
				auto &s(t._stack);
				if (Int(s.size()) <= t._stack_offs) { throw Error("Nothing to isa"); }
				const bool ok(s.back().isa(rhs));
				s.pop_back();
				env.push(env.bool_type, ok);
				env.jump(op.next);
			};
		}

		OpImp Jump::Type::make_imp(Env &env, Op &op) const {
			const auto &end_pc(op.as<ops::Jump>().end_pc);
			return [&env, &end_pc]() { env.jump(end_pc); };
		}

		OpImp JumpIf::Type::make_imp(Env &env, Op &op) const {
			auto &o(op.as<ops::JumpIf>());
			
			return [&env, &op, &o]() {
				if (o.cond()) {
					env.jump(o.end_pc);
				} else {
					env.jump(op.next);
				}
			};
		}

		OpImp Lambda::Type::make_imp(Env &env, Op &op) const {			
			auto &o(op.as<ops::Lambda>());
			
			return [&env, &o]() {
				env.push(env.lambda_type,
								 make_shared<snabl::Lambda>(o.is_scope ? env.scope() : nullptr,
																						o.start_pc, o.end_pc));
				env.jump(o.end_pc);
			};
		}

		OpImp Let::Type::make_imp(Env &env, Op &op) const {
			const auto &id(op.as<ops::Let>().id);
			
			return [&env, &op, &id]() {
				auto &t(*env._task);
				auto &s(t._stack);
				if (Int(s.size()) <= t._stack_offs) { throw Error("Nothing to let"); }
				auto &v(s.back());
				env.scope()->let(id, v);
				s.pop_back();
				env.jump(op.next);
			};
		}

		OpImp Nop::Type::make_imp(Env &env, Op &op) const {
			return [&env, &op]() { env.jump(op.next); };
		}

		void Push::Type::dump_data(const Push &op, ostream &out) const {
			out << ' ';
			op.val.dump(out);
		}

		OpImp Push::Type::make_imp(Env &env, Op &op) const {
			const auto &v(op.as<ops::Push>().val);
			
			return [&env, &op, &v]() {
				env.push(v);
				env.jump(op.next);
			};
		}

		OpImp Recall::Type::make_imp(Env &env, Op &op) const {
			return [&env, &op]() { env.recall(op.pos); };
		}

		OpImp Return::Type::make_imp(Env &env, Op &op) const {
			return [&env, &op]() { env._return(op.pos); };
		}

		OpImp Rot::Type::make_imp(Env &env, Op &op) const {
			return [&env, &op]() {
				auto &t(*env._task);
				auto &s(t._stack);
				if (Int(s.size()) <= t._stack_offs+2) { throw Error("Nothing to rot"); }
				auto i(s.size()-1);
				swap(s[i], s[i-2]);
				swap(s[i], s[i-1]);
				env.jump(op.next);
			};
		}

		OpImp RSwap::Type::make_imp(Env &env, Op &op) const {
			return [&env, &op]() {
				auto &t(*env._task);
				auto &s(t._stack);
				if (Int(s.size()) <= t._stack_offs+2) { throw Error("Nothing to rswap"); }
				const auto i(s.size()-1);
				swap(s[i], s[i-2]);
				env.jump(op.next);
			};
		}

		OpImp Scope::Type::make_imp(Env &env, Op &op) const {
			return [&env, &op]() {
				env.begin_scope(env.scope());
				env.jump(op.next);
			};
		}

		OpImp ScopeEnd::Type::make_imp(Env &env, Op &op) const {
			return [&env, &op]() {
				env.end_scope();
				env.jump(op.next);
			};
		}

		OpImp SDrop::Type::make_imp(Env &env, Op &op) const {
			return [&env, &op]() {
				auto &t(*env._task);
				auto &s(t._stack);
				if (Int(s.size()) <= t._stack_offs+1) { throw Error("Nothing to sdrop"); }
				const auto i(s.size()-1);
				s[i-1] = s[i];	
				s.pop_back();
				env.jump(op.next);
			};
		}
		
		OpImp Split::Type::make_imp(Env &env, Op &op) const {
			const auto offs(op.as<Split>().offs);
			
			return [&env, &op, offs]() {
				env.begin_split(offs);
				env.jump(op.next);
			};
		}

		OpImp SplitEnd::Type::make_imp(Env &env, Op &op) const {
			return [&env, &op]() {
				env.end_split();
				env.jump(op.next);
			};
		}

		OpImp Stack::Type::make_imp(Env &env, Op &op) const {
			const auto end_split(op.as<Stack>().end_split);
			
			return [&env, &op, end_split]() {
				auto &t(*env._task);
				auto &s(t._stack);

				const Int offs(t._stack_offs);
				if (end_split) { env.end_split(); }
				auto ss(make_shared<snabl::Stack>());
				
				if (Int(s.size()) > offs) {
					const auto i(s.begin()+offs), j(s.end());
					move(i, j, back_inserter(*ss));
					s.erase(i, j);
				}
				
				env.push(env.stack_type, ss);
				env.jump(op.next);
			};
		}

		OpImp Stop::Type::make_imp(Env &env, Op &op) const {
			return [&env]() { env.jump(nullptr); };
		}

		OpImp Swap::Type::make_imp(Env &env, Op &op) const {
			return [&env, &op]() {
				auto &t(*env._task);
				auto &s(t._stack);
				if (Int(s.size()) <= t._stack_offs+1) { throw Error("Nothing to swap"); }
				const auto i(s.size()-1);
				swap(s[i], s[i-1]);
				env.jump(op.next);
			};
		}

		OpImp Task::Type::make_imp(Env &env, Op &op) const {			
			auto &o(op.as<ops::Task>());
			
			return [&env, &o]() {
				auto start_pc(&(env._ops.begin() + o.start_pc)->imp);
				auto parent_scope(o.is_scope ? env._task->_scope : nullptr);
				env.push(env.task_type, env.start_task(start_pc, parent_scope));
				env.jump(o.end_pc);
			};
		}

		OpImp Times::Type::make_imp(Env &env, Op &op) const {
			auto &o(op.as<ops::Times>());

			return [&env, &op, &o]() {
				env.let_reg(o.i_reg, env.pop().as<Int>());
				env.jump(op.next);
			};
		}

		OpImp Try::Type::make_imp(Env &env, Op &op) const {			
			auto &o(op.as<ops::Try>());

			return [&env, &op, &o]() {
				env.let_reg(o.state_reg, State(env));
				env.begin_try(o);
				env.jump(op.next);
			};
		}

		OpImp TryEnd::Type::make_imp(Env &env, Op &op) const {
			auto &o(op.as<ops::TryEnd>());

			return [&env, &op, &o]() {
				env.clear_reg(o.state_reg);
				env.end_try();
				env.jump(op.next);
			};
		}

		OpImp Yield::Type::make_imp(Env &env, Op &op) const {
			return [&env, &op]() {
				env.jump(op.next);
				env.yield();
			};
		}
	}
}
