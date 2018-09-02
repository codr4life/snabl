#include <cmath>

#include "snabl/call.hpp"
#include "snabl/env.hpp"
#include "snabl/fimp.hpp"
#include "snabl/lib.hpp"

namespace snabl {
	Sym Fimp::get_id(const Func &func, const Args &args) {
		stringstream buf;
		buf << func.id.name() << '<';
		char sep = 0;

		for (auto &a: args) {
			if (sep) {
				buf << sep;
			} else {
				sep = ' ';
			}

			if (a.is_undef()) {
				buf << a.type()->id.name();
			} else {
				a.write(buf);
			}
		}

		buf << '>';
		return func.lib.env.sym(buf.str());
	}

	bool Fimp::compile(Pos pos) {
		auto &env(func->lib.env);
		if (_start_pc) { return false; }
		auto &skip(env.emit(ops::Skip::type, pos, 0).as<ops::Skip>());
		_start_pc = env.ops.end();
		env.compile(forms);
		
		_has_vars = (find_if(*_start_pc, env.ops.end(), [](const Op &op) {
					return &op.type == &ops::PutVar::type;
				}) != env.ops.end());

		env.emit(ops::FimpRet::type, pos, _has_vars);
		_nops = skip.nops = env.ops.end()-*_start_pc;
		return true;
	}

	void Fimp::call(const FimpPtr &fimp, Pos pos) {
		const auto &func(fimp->func);
		auto &env(func->lib.env);
		env.begin_stack(env.stack().size()-func->nargs);
		
		if (fimp->imp) {
			auto &call(env.begin_call(*env.scope(), *fimp));
			(*fimp->imp)(call);
			auto stack_offs(env.end_stack());
			
			if (env.stack().size() != stack_offs+func->nrets) {
				throw Error(fmt("Invalid return stack: %0", {fimp->id}));
			}

			env.end_call();
		} else {
			fimp->compile(pos);
			auto &scope(fimp->_has_vars ? env.begin_scope() : env.scope());
			env.begin_call(*scope, *fimp, env.pc);
			env.pc = *fimp->_start_pc;
		}
	}

	Fimp::Fimp(const FuncPtr &func, const Args &args, const Rets &rets, Imp imp):
		id(get_id(*func, args)), func(func), args(args), rets(rets), imp(imp),
	  _start_pc(nullopt), _nops(0), _has_vars(false) { }

	Fimp::Fimp(const FuncPtr &func,
						 const Args &args, const Rets &rets,
						 Forms::const_iterator begin,
						 Forms::const_iterator end):
		id(get_id(*func, args)), func(func), args(args), rets(rets), forms(begin, end),
		_start_pc(nullopt), _nops(0), _has_vars(false) { }

	optional<size_t> Fimp::score(const Stack &stack) const {
		if (!func->nargs) { return 0; }
		if (stack.size() < func->nargs) { return nullopt; }
		auto &env(func->lib.env);
		auto i(next(stack.begin(), stack.size()-func->nargs));
		auto j(args.begin());
		size_t score(0);

		for (; j != args.end(); i++, j++) {
			auto &iv(*i), &jv(*j);
			auto &it(iv.type()), &jt(jv.type());
			if (it == env.no_type) { continue; }

			if (jv.is_undef()) {
				if (!it->isa(jt)) { return nullopt; }
			} else {
				if (iv.is_undef() || !iv.is_eqval(jv)) { return nullopt; }
			}
			
			score += abs(it->tag-jt->tag);
		}

		return score;
	}
}
