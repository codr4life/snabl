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
		auto &start(env.emit(ops::Fimp::type, pos, fip));
		const auto start_offs(env.ops().size());
		env.compile(*fi.form);

		for (auto op(env.ops().begin()+start_offs);
				 op != env.ops().end();
				 op++) {
			if (&op->type == &ops::Get::type || &op->type == &ops::Let::type) {
				fi._opts |= Opts::Vars;
			}

			if (&op->type == &ops::Recall::type) { fi._opts |= Opts::Recalls; }
		}
		
		auto &end(env.emit(ops::Return::type, pos));
		fi._start_pc = *start.next;
		
		fi._end_pc = [&env, &end, &fi]() {
			auto pc(end.next);
			env.jump(pc);
			if (pc) { fi._end_pc = *pc; }
		};

		return true;
	}

	void Fimp::call(const FimpPtr &fip, Pos pos) {
		auto &fi(*fip);
		const auto &fn(*fi.func);
		auto &env(fn.lib.env);
		
		if (fi.imp) {
			Target::begin_call(fip, env, pos);
			fi.imp(fi);
			fi.end_call(env);
		} else {
			Fimp::compile(fip, pos);
			if (fi._opts & Opts::Vars) { env.begin_scope(fi._parent_scope); }
			Target::begin_call(fip, env, pos, env.pc());
			env.split(fn.nargs);		
			env.jump(&fi._start_pc);
		}
	}

	Fimp::Fimp(const FuncPtr &func, const Args &args, Imp imp):
		Def(get_id(*func, args)), func(func), args(args), imp(imp) { }

	Fimp::Fimp(const FuncPtr &func, const Args &args, const Form &form):
		Def(get_id(*func, args)), func(func), args(args), form(form) { }

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
