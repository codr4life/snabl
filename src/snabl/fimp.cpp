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

	bool Fimp::compile(const FimpPtr &fip, Pos pos) {
		auto &fi(*fip);
		if (fi._start_pc) { return false; }
		auto &env(fi.func->lib.env);
		auto &start_op(env.emit(ops::Fimp::type, pos, fip));
		env.begin_regs();
		const auto offs(env.ops().size());
		env.compile(*fi.form);
		if (env.end_regs()) { fi._opts |= Opts::Regs; }

		for (auto op(env.ops().begin()+offs);
				 op != env.ops().end();
				 op++) {
			if (&op->type == &ops::Get::type || &op->type == &ops::Let::type) {
				fi._opts |= Opts::Vars;
			}

			if (&op->type == &ops::Recall::type) { fi._opts |= Opts::Recalls; }
		}
		
		env.emit(ops::Return::type, pos);
		fi._start_pc = start_op.next;
		fi._end_pc = env.ops().size();
		return true;
	}

	void Fimp::call(const FimpPtr &fip, Pos pos) {
		auto &fi(*fip);
		const auto &fn(*fi.func);
		auto &env(fn.lib.env);
		
		if (fi.imp) {
			env.begin_call(fip, pos, env.pc());
			fi.imp(fi);
			env.end_call();
		} else {
			Fimp::compile(fip, pos);
			if (fi._parent_scope) { env.begin_scope(fi._parent_scope); }
			env.begin_split(fn.nargs);		
			env.begin_call(fip, pos, env.pc());
			env.jump(fi._start_pc);
		}
	}

	Fimp::Fimp(const FuncPtr &func, const Args &args, Imp imp):
		Def(get_id(*func, args)), func(func), args(args), imp(imp) { }

	Fimp::Fimp(const FuncPtr &func, const Args &args, const Form &form):
		Def(get_id(*func, args)), func(func), args(args), form(form) { }

	Int Fimp::score(Stack::const_iterator begin, Stack::const_iterator end) const {
		auto &env(func->lib.env);
		auto i(begin), j(args.begin());
		Int score(0);

		for (; j != args.end(); i++, j++) {
			if (i == end) { return -1; }
			
			auto &iv(*i), &jv(*j);
			auto &it(iv.type()), &jt(jv.type());
			if (it == env.no_type) { continue; }

			if (jv.has_val()) {
				if (!iv.has_val() || !iv.eqval(jv)) { return -1; }
			} else if (!it->isa(jt)) {
				return -1;
			}
			
			score += abs(static_cast<Int>(it->tag-jt->tag));
		}

		return score;
	}
}
