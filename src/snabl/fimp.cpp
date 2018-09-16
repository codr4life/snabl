#include "snabl/call.hpp"
#include "snabl/env.hpp"
#include "snabl/fimp.hpp"
#include "snabl/lib.hpp"
#include "snabl/run.hpp"

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

			if (a.has_val()) {
				a.write(buf);
			} else {
				buf << a.type()->id.name();
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
		env.compile(*fimp->form);

		for (auto op(env.ops.begin() + *fimp->_start_pc);
				 op != env.ops.end();
				 op++) {
			if (&op->type == &ops::Get::type || &op->type == &ops::Let::type) {
				fimp->_opts |= Opts::Vars;
			}

			if (&op->type == &ops::Recall::type) { fimp->_opts |= Opts::Recalls; }
		}
		
		env.emit(ops::FimpEnd::type, pos, fimp->_opts & Opts::Vars);
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
			if (fimp->_is_calling) {
				throw RuntimeError(env, pos, fmt("Recursive call: %0", {fimp->id}));
			}

			fimp->compile(fimp, pos);
			auto &scope((fimp->_opts & Opts::Vars)
									? env.begin_scope(fimp->_parent_scope)
									: env.scope());
			env.begin_call(*scope, pos, fimp, env.pc-env.ops.begin());
			env.split(func->nargs);		
			fimp->_is_calling = true;
			env.pc = env.ops.begin() + *fimp->_start_pc;
		}
	}

	Fimp::Fimp(const FuncPtr &func, const Args &args, Imp imp):
		Def(get_id(*func, args)), func(func), args(args), imp(imp),
	  _nops(0), _opts(Opts::None), _is_calling(false) { }

	Fimp::Fimp(const FuncPtr &func, const Args &args, const Form &form):
		Def(get_id(*func, args)), func(func), args(args), form(form),
		_nops(0), _opts(Opts::None), _is_calling(false) { }

	optional<size_t> Fimp::score(Stack::const_iterator begin,
															 Stack::const_iterator end) const {
		auto &env(func->lib.env);
		auto i(begin), j(args.begin());
		size_t score(0);

		for (; j != args.end(); i++, j++) {
			if (i == end) { return nullopt; }
			
			auto &iv(*i), &jv(*j);
			auto &it(iv.type()), &jt(jv.type());
			if (it == env.no_type) { continue; }

			if (jv.has_val()) {
				if (!iv.has_val() || !iv.eqval(jv)) { return nullopt; }
			} else {
				if (!it->isa(jt)) { return nullopt; }
			}
			
			score += abs(static_cast<ssize_t>(it->tag-jt->tag));
		}

		return score;
	}
}
