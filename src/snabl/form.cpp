#include "snabl/env.hpp"
#include "snabl/form.hpp"
#include "snabl/run.hpp"

namespace snabl {
	AFormType::AFormType(string_view id): id(id) { }

	FormImp::~FormImp() { }

	Form::Form(const Form &source):
		type(source.type),
		pos(source.pos),
		imp(source.imp->clone()) { }

	namespace forms {
		const FormType<Comma> Comma::type("comma");
		const FormType<Fimp> Fimp::type("fimp");
		const FormType<Id> Id::type("id");
		const FormType<Lambda> Lambda::type("lambda");
		const FormType<Lit> Lit::type("lit");
		const FormType<Query> Query::type("query");
		const FormType<Semi> Semi::type("semi");
		const FormType<Sexpr> Sexpr::type("sexpr");
		const FormType<Stack> Stack::type("stack");

		FormImp *Comma::clone() const { return new Comma(body.begin(), body.end()); }

		void Comma::dump(ostream &out) const {
			out << ", ";
			char sep(0);

			for (auto &f: body) {
				if (sep) { out << sep; }
				f.imp->dump(out);
				sep = ' ';
			}
		}		

		void Comma::compile(Forms::const_iterator &in, Forms::const_iterator end,
												FuncPtr &func, FimpPtr &fimp,
												Env &env) const {
			auto &sexpr((*in++).as<Comma>());
			env.compile(sexpr.body);
		}

		Fimp::Fimp(Sym id,
							 Forms::const_iterator begin,
							 Forms::const_iterator end): id(id) {
			transform(begin, end, back_inserter(type_ids),
								[](const Form &f) -> Sym { return f.as<Id>().id; });
		}

		Fimp::Fimp(Sym id, const Ids &type_ids): id(id), type_ids(type_ids) { }

		FormImp *Fimp::clone() const { return new Fimp(id, type_ids); }

		void Fimp::dump(ostream &out) const {
			out << id << '<';
			char sep(0);

			for (auto &id: type_ids) {
				if (sep) { out << sep; }
				out << id.name();
				sep = ' ';
			}
			
			out << '>';
		}
		
		void Fimp::compile(Forms::const_iterator &in, Forms::const_iterator end,
											 FuncPtr &func, FimpPtr &fimp,
											 Env &env) const {
			auto &lib(env.lib());
			auto pos(in->pos);
			auto &form((in++)->as<Fimp>());
			env.compile(Form(Id::type, pos, form.id), func, fimp);
			if (!func) { throw CompileError(pos, "Missing func"); }
			snabl::Stack args;
				
			transform(form.type_ids.begin(), form.type_ids.end(), back_inserter(args),
								[&lib, pos](Sym id) {
									auto t(lib.get_type(id));
									
									if (!t) {
										throw CompileError(pos, "Unknown type: " + id.name());
									}
									
									return Box(*t);
								});
			
			
			if (Int(args.size()) != func->nargs) {
				throw CompileError(pos, fmt("Wrong number of args: %0", {func->id}));
			}
			
			auto fi(func->get_best_fimp(args.begin(), args.end()));
			if (!fi) { throw CompileError(pos, fmt("Unknown fimp: %0", {func->id})); }
			fimp = *fi;
		}

		Id::Id(Sym id): id(id) { }

		FormImp *Id::clone() const { return new Id(id); }

		void Id::dump(ostream &out) const { out << id.name(); }
		
		void Id::compile(Forms::const_iterator &in,
										 Forms::const_iterator end,
										 FuncPtr &func, FimpPtr &fimp,
										 Env &env) const {
			auto &form(*in);
			auto &id(form.as<Id>().id);

			if (id.name().front() == '@') {
				in++;
				env.emit(ops::Get::type, form.pos, env.sym(id.name().substr(1)));
			} else if (isupper(id.name().front())) {
				in++;
				auto t(env.lib().get_type(id));
				if (!t) { throw CompileError(form.pos, fmt("Unknown type: %0", {id})); }
				env.emit(ops::Push::type, form.pos, env.meta_type, *t);
			} else {
				auto &lib(env.lib());
				auto m(lib.get_macro(id));
				
				if (m) {
					(*m)->call(in, end, func, fimp, env);
				} else {
					in++;
					auto fn(lib.get_func(id));

					if (!fn) {
						throw CompileError(form.pos, fmt("Unknown id: '%0'", {id.name()}));
					}
					
					if (func) {
						throw CompileError(form.pos,fmt("Extra func: %0", {(*fn)->id}));
					}
						
					func = *fn;
				}
			}
		}

		FormImp *Lambda::clone() const { return new Lambda(body.begin(), body.end()); }

		void Lambda::dump(ostream &out) const {
			out << '{';
			char sep(0);

			for (auto &f: body) {
				if (sep) { out << sep; }
				f.imp->dump(out);
				sep = ' ';
			}
			
			out << '}';
		}		

		void Lambda::compile(Forms::const_iterator &in,
												 Forms::const_iterator end,
												 FuncPtr &func, FimpPtr &fimp,
												 Env &env) const {
			auto &f(*in++);
			auto &l(f.as<Lambda>());
			auto &start_op(env.emit(ops::Lambda::type, f.pos));
			auto &start(start_op.as<ops::Lambda>());
			env.begin_regs();
			const auto offs(env.ops().size());
			env.compile(l.body);
			if (env.end_regs()) { start.opts |= Target::Opts::Regs; }
			
			for (auto bop(env.ops().begin()+offs);
					 bop != env.ops().end();
					 bop++) {
				if (&bop->type == &ops::Get::type || &bop->type == &ops::Let::type) {
					start.opts |= Target::Opts::Vars;
				}
				
				if (&bop->type == &ops::Recall::type) {
					start.opts |= Target::Opts::Recalls;
				}
			}
			
			env.emit(ops::Return::type, f.pos);
			start.start_pc = *start_op.next;
			start.end_pc = env.ops().size();
		}
		
		Lit::Lit(const Box &val): val(val) { }

		FormImp *Lit::clone() const { return new Lit(val); }

		void Lit::dump(ostream &out) const { val.dump(out); }

		void Lit::compile(Forms::const_iterator &in,
											Forms::const_iterator end,
											FuncPtr &func, FimpPtr &fimp,
											Env &env) const {
			auto &form(*in++);
			env.emit(ops::Push::type, form.pos, form.as<Lit>().val);
		}

		Query::Query(const Form &form): form(form) {}
		
		FormImp *Query::clone() const {
			return new Query(form);
		}

		void Query::dump(ostream &out) const {
			form.imp->dump(out);
			out << '?';
		}		

		void Query::compile(Forms::const_iterator &in, Forms::const_iterator end,
												FuncPtr &func, FimpPtr &fimp,
												Env &env) const {
			auto &form(*in);
			auto &qf(form.as<forms::Query>().form);
			
			if (&qf.type == &forms::Lit::type) {
				env.emit(ops::Eqval::type, qf.pos, qf.as<Lit>().val);
			} else if (&qf.type == &forms::Id::type) {
				if (isupper(qf.as<forms::Id>().id.name().front())) {
					auto &id(qf.as<forms::Id>().id);
					auto t(env.lib().get_type(id));
					if (!t) { throw CompileError(qf.pos, fmt("Unknown type: %0", {id})); }
					env.emit(ops::Isa::type, qf.pos, *t);
				} else {
					env.compile(qf, func, fimp);
					env.emit(ops::Eqval::type, qf.pos);
				}
			} else {
				throw CompileError(qf.pos, fmt("Invalid query: %0", {qf.type.id}));
			}

			in++;
		}

		FormImp *Semi::clone() const { return new Semi(body.begin(), body.end()); }

		void Semi::dump(ostream &out) const {
			out << "; ";
			char sep(0);

			for (auto &f: body) {
				if (sep) { out << sep; }
				f.imp->dump(out);
				sep = ' ';
			}
		}		

		void Semi::compile(Forms::const_iterator &in, Forms::const_iterator end,
											 FuncPtr &func, FimpPtr &fimp,
											 Env &env) const {
			auto &form(*in++);
			if (!func) { throw SyntaxError(form.pos, "Missing func"); }
			env.emit(form.pos, func, fimp);
			env.compile(form.as<Semi>().body);
		}

		FormImp *Sexpr::clone() const { return new Sexpr(body.begin(), body.end()); }

		void Sexpr::dump(ostream &out) const {
			out << '(';
			char sep(0);

			for (auto &f: body) {
				if (sep) { out << sep; }
				f.imp->dump(out);
				sep = ' ';
			}
			
			out << ')';
		}		

		void Sexpr::compile(Forms::const_iterator &in, Forms::const_iterator end,
												FuncPtr &func, FimpPtr &fimp,
												Env &env) const {
			auto &f(*in++);
			auto &b(f.as<Sexpr>().body);

			bool split(!b.empty() &&
								 &b.front().type == &forms::Id::type &&
								 b.front().as<forms::Id>().id == env.sym("|"));

			if (split) { env.emit(ops::Split::type, f.pos); }
			env.compile(split ? b.begin()+1 : b.begin(), b.end());
			if (split) { env.emit(ops::SplitEnd::type, f.pos); }
		}
		
		FormImp *Stack::clone() const { return new Stack(body.begin(), body.end()); }

		void Stack::dump(ostream &out) const {
			out << '[';
			char sep(0);

			for (auto &f: body) {
				if (sep) { out << sep; }
				f.imp->dump(out);
				sep = ' ';
			}
			
			out << ']';
		}	

		void Stack::compile(Forms::const_iterator &in, Forms::const_iterator end,
												FuncPtr &func, FimpPtr &fimp,
												Env &env) const {
			auto &f(*in++);
			auto &b(f.as<Stack>().body);

			bool split(b.empty() ||
								 &b.front().type != &forms::Id::type ||
								 b.front().as<forms::Id>().id != env.sym(".."));
			
			if (split) { env.emit(ops::Split::type, f.pos); }
			env.compile(split ? b.begin() : b.begin()+1, b.end());
			env.emit(ops::Stack::type, f.pos, split);
		}
	}
}
