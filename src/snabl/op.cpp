#include "snabl/env.hpp"
#include "snabl/fimp.hpp"
#include "snabl/func.hpp"
#include "snabl/lambda.hpp"
#include "snabl/op.hpp"

namespace snabl {
	size_t AOpType::next_label_offs(0);

  optional<function<void ()>> AOpType::run(Op &op,
																					 optional<function<void ()>> &next,
																					 Ops::const_iterator end_pc,
																					 Env &env) const {
		return [&next, end_pc, &env]() {
			next = (++env.pc == end_pc)
				? nullopt
				: env.pc->type.run(*env.pc, next, end_pc, env);
		};
	}

	namespace ops {
		const OpType<Call> Call::type("call");
		const OpType<DDrop> DDrop::type("ddrop");
		const Drop::Type Drop::type("drop");
		const OpType<Dup> Dup::type("dup");
		const OpType<Else> Else::type("else");
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
		const OpType<RSwap> RSwap::type("rswap");
		const OpType<SDrop> SDrop::type("sdrop");
		const Skip::Type Skip::type("skip");
		const OpType<Split> Split::type("split");
		const OpType<SplitEnd> SplitEnd::type("split-end");
		const OpType<Stack> Stack::type("stack");
		const OpType<Swap> Swap::type("swap");
		const Try::Type Try::type("try");
		const TryEnd::Type TryEnd::type("try-end");

		optional<function<void ()>> Drop::Type::run(Op &op,
																								optional<function<void ()>> &next,
																								Ops::const_iterator end_pc,
																								Env &env) const {
			return [&next, end_pc, &env]() {
				if (env._stack.size() <= env._stack_offs) { throw Error("Nothing to drop"); }
				env._stack.pop_back();
				
				next = (++env.pc == end_pc)
					? nullopt
					: env.pc->type.run(*env.pc, next, end_pc, env);
			};
		};

		void Eqval::Type::dump(const Eqval &op, ostream &out) const {
			if (op.rhs) {
				out << ' ';
				op.rhs->dump(out);
			}
		}

		void Fimp::Type::dump(const Fimp &op, ostream &out) const {
			out << ' ' << op.ptr->id << ' ' << op.ptr->nops();
		}

		Funcall::Funcall(const FuncPtr &func): func(func) { }
		Funcall::Funcall(const FimpPtr &fimp): func(fimp->func), fimp(fimp) { }

		void Funcall::Type::dump(const Funcall &op, ostream &out) const {
			out << ' ' << (op.fimp ? op.fimp->id : op.func->id);
			if (op.prev_fimp) { out << " (" << op.prev_fimp->id << ')'; }
		}

		optional<function<void ()>> Funcall::Type::run(Op &op,
																									 optional<function<void ()>> &next,
																									 Ops::const_iterator end_pc,
																									 Env &env) const {
			auto &o(op.as<ops::Funcall>());
			
			return [&o, &next, end_pc, &env]() {
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
				
				next = (env.pc == end_pc)
					? nullopt
					: env.pc->type.run(*env.pc, next, end_pc, env);
			};
		};

		void Isa::Type::dump(const Isa &op, ostream &out) const {
			out << ' ' << op.rhs->id;
		}

		optional<function<void ()>> Lambda::Type::run(Op &op,
																									optional<function<void ()>> &next,
																									Ops::const_iterator end_pc,
																									Env &env) const {
			auto &o(op.as<ops::Lambda>());
			
			return [&o, &next, end_pc, &env]() {
				env.push(env.lambda_type,
								 make_shared<snabl::Lambda>((o.opts & Target::Opts::Vars)
																						? env.scope()
																						: nullptr,
																						*o.start_pc, *o.nops,
																						o.opts));
				env.pc += *o.nops+1;

				next = (env.pc == end_pc)
					? nullopt
					: env.pc->type.run(*env.pc, next, end_pc, env);
			};
		};

		optional<function<void ()>> LambdaEnd::Type::run(Op &op,
																										 optional<function<void ()>>
																										 &next,
																										 Ops::const_iterator end_pc,
																										 Env &env) const {
			return [&next, end_pc, &env]() {
				const auto &c(env.call());
				const auto &l(*dynamic_pointer_cast<snabl::Lambda>(c.target));
				if (l.opts() & Target::Opts::Vars) { env.end_scope(); }
				env.pc = *c.return_pc;
				env.end_call();

				next = (env.pc == end_pc)
					? nullopt
					: env.pc->type.run(*env.pc, next, end_pc, env);
			};
		};

		void Push::Type::dump(const Push &op, ostream &out) const {
			out << ' ';
			op.val.dump(out);
		}

		optional<function<void ()>> Push::Type::run(Op &op,
																								optional<function<void ()>> &next,
																								Ops::const_iterator end_pc,
																								Env &env) const {
			return [&op, &next, end_pc, &env]() {
				env.push(op.as<ops::Push>().val); 
				
				next = (++env.pc == end_pc)
					? nullopt
					: env.pc->type.run(*env.pc, next, end_pc, env);
			};
		};

		optional<function<void ()>> Try::Type::run(Op &op,
																							 optional<function<void ()>> &next,
																							 Ops::const_iterator end_pc,
																							 Env &env) const {
			auto &o(op.as<ops::Try>());
			
			return [&o, &next, end_pc, &env]() {
				o.state.emplace(env);
				env._tries.push_back(&o);
				
				next = (++env.pc == end_pc)
					? nullopt
					: env.pc->type.run(*env.pc, next, end_pc, env);
			};
		};

		optional<function<void ()>> TryEnd::Type::run(Op &op,
																									optional<function<void ()>> &next,
																									Ops::const_iterator end_pc,
																									Env &env) const {
			return [&next, end_pc, &env]() {
				env._tries.back()->state.reset();
				env._tries.pop_back();
				
				next = (++env.pc == end_pc)
					? nullopt
					: env.pc->type.run(*env.pc, next, end_pc, env);
			};
		};
	}
}
