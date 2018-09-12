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

	bool Fimp::compile(const FimpPtr &fimp, Pos pos) {
		auto &env(fimp->func->lib.env);
		if (fimp->_start_pc) { return false; }
		env.emit(ops::Fimp::type, pos, fimp);
		fimp->_start_pc = env.ops.size();
		env.compile(fimp->forms);

		for (auto op(env.ops.begin() + *fimp->_start_pc);
				 op != env.ops.end();
				 op++) {
			if (&op->type == &ops::Get::type || &op->type == &ops::Let::type) {
				fimp->_has_vars = true;
			}

			if (&op->type == &ops::Recall::type) {
				fimp->_has_recalls = true;
			}
		}
		
		env.emit(ops::FimpRet::type, pos, fimp->_has_vars);
		fimp->_nops = env.ops.size() - *fimp->_start_pc;
		return true;
	}

	void Fimp::call(const FimpPtr &fimp, Pos pos) {
		const auto &func(fimp->func);
		auto &env(func->lib.env);
		
		if (fimp->imp) {
			auto &call(env.begin_call(*env.scope(), pos, fimp));
			(*fimp->imp)(call);
			env.end_call();
		} else {
			fimp->compile(fimp, pos);
			auto &scope(fimp->_has_vars ? env.begin_scope() : env.scope());
			env.begin_call(*scope, pos, fimp, env.pc);
			env.pc = env.ops.begin() + *fimp->_start_pc;
		}
	}

	Fimp::Fimp(const FuncPtr &func, const Args &args, const Rets &rets, Imp imp):
		Def(get_id(*func, args)), func(func), args(args), rets(rets), imp(imp),
	  _start_pc(nullopt), _nops(0), _has_vars(false), _has_recalls(false) { }

	Fimp::Fimp(const FuncPtr &func,
						 const Args &args, const Rets &rets,
						 Forms::const_iterator begin,
						 Forms::const_iterator end):
		Def(get_id(*func, args)), func(func), args(args), rets(rets), forms(begin, end),
		_start_pc(nullopt), _nops(0), _has_vars(false), _has_recalls(false) { }

	optional<size_t> Fimp::score(const Stack &stack) const {
		if (!func->nargs) { return 0; }
		if (stack.size() < func->nargs) { return nullopt; }
		auto &env(func->lib.env);
		auto i(stack.begin()+stack.size()-func->nargs);
		auto j(args.begin());
		size_t score(0);

		for (; j != args.end(); i++, j++) {
			auto &iv(*i), &jv(*j);
			auto &it(iv.type()), &jt(jv.type());
			if (it == env.no_type) { continue; }

			if (jv.is_undef()) {
				if (!it->isa(jt)) { return nullopt; }
			} else {
				if (iv.is_undef() || !iv.eqval(jv)) { return nullopt; }
			}
			
			score += abs(static_cast<ssize_t>(it->tag-jt->tag));
		}

		return score;
	}
}
