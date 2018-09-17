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
		const FormType<Comma> Comma::type("Comma");
		const FormType<Id> Id::type("Id");
		const FormType<Lambda> Lambda::type("Lambda");
		const FormType<Lit> Lit::type("Lit");
		const FormType<Query> Query::type("Query");
		const FormType<Semi> Semi::type("Semi");
		const FormType<Sexpr> Sexpr::type("Sexpr");
		const FormType<Stack> Stack::type("Stack");
		const FormType<TypeList> TypeList::type("TypeList");

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
					
					if ((*fn)->nargs) {
						if (func) {
							throw CompileError(form.pos,fmt("Extra func: %0", {(*fn)->id}));
						}
						
						func = *fn;

						if (in != end && &in->type == &TypeList::type) {
							auto &ids((in++)->as<TypeList>().ids);
							snabl::Stack args;
							
							transform(ids.begin(), ids.end(), back_inserter(args),
												[&form, &lib](Sym id) {
													auto t(lib.get_type(id));

													if (!t) {
														throw CompileError(form.pos,
																							 "Unknown type: " + id.name());
													}
													
													return Box(*t);
												});


							if (args.size() != (*fn)->nargs) {
								throw CompileError(form.pos,
																	 fmt("Wrong number of args: %0", {(*fn)->id}));
							}
							
							auto fi((*fn)->get_best_fimp(args.begin(), args.end()));

							if (!fi) {
								throw CompileError(form.pos, fmt("Unknown fimp: %0", {(*fn)->id}));
							}
							
							fimp = *fi;
						}
					} else {
						auto &fi((*fn)->get_fimp());
						if (!fi->imp) { Fimp::compile(fi, form.pos); }
						env.emit(ops::Funcall::type, form.pos, fi);
					}
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
			const auto start_offs(env.ops.size());
			env.compile(l.body);

			for (auto bop(env.ops.begin()+start_offs);
					 bop != env.ops.end();
					 bop++) {
				if (&bop->type == &ops::Get::type || &bop->type == &ops::Let::type) {
					start.opts |= Target::Opts::Vars;
				}
				
				if (&bop->type == &ops::Recall::type) {
					start.opts |= Target::Opts::Recalls;
				}
			}
			
			auto &end_op(env.emit(ops::LambdaEnd::type, f.pos));
			start.start_pc = *start_op.next;
			start.end_pc = [&env, &end_op]() { env.pc = end_op.next; };
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

		TypeList::TypeList(Forms::const_iterator begin, Forms::const_iterator end) {
			transform(begin, end, back_inserter(ids),
								[](const Form &f) -> Sym { return f.as<Id>().id; });
		}

		TypeList::TypeList(const Ids &ids): ids(ids) { }

		FormImp *TypeList::clone() const { return new TypeList(ids); }

		void TypeList::dump(ostream &out) const {
			out << '<';
			char sep(0);

			for (auto &id: ids) {
				if (sep) { out << sep; }
				out << id.name();
				sep = ' ';
			}
			
			out << '>';
		}
		
		void TypeList::compile(Forms::const_iterator &in, Forms::const_iterator end,
													 FuncPtr &func, FimpPtr &fimp,
													 Env &env) const {
			throw CompileError(in->pos, "Stray type list");
		}		
	}
}
