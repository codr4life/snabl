#include <algorithm>

#include "snabl/env.hpp"
#include "snabl/form.hpp"

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
		const FormType<Literal> Literal::type("Literal");
		const FormType<Semicolon> Semicolon::type("Semicolon");
		const FormType<Sexpr> Sexpr::type("Sexpr");
		const FormType<TypeList> TypeList::type("TypeList");

		Comma::Comma(Forms::const_iterator begin, Forms::const_iterator end):
			body(begin, end) { }

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
				env.emit(ops::GetVar::type, form.pos, env.sym(id.name().substr(1)));
			} else if (id.name().front() == '$') {
				in++;
				env.emit(ops::Push::type,
								 form.pos,
								 env.sym_type,
								 env.sym(id.name().substr(1)));
			} else {
				auto &lib(env.lib());
				auto m(lib.get_macro(id));
				
				if (m) {
					(*m)->call(in, end, func, fimp, env);
				} else {
					in++;
					auto fn(lib.get_func(id));
					if (!fn) { throw Error("Unknown id: " + id.name()); }
					
					if ((*fn)->nargs) {
						if (func) { throw Error("Extra func"); }
						func = *fn;

						if (in != end && &in->type == &TypeList::type) {
							auto &ids((in++)->as<TypeList>().ids);
							Stack args;
							transform(ids.begin(), ids.end(), back_inserter(args),
												[&lib](Sym id) {
													auto t(lib.get_type(id));
													if (!t) { throw Error("Unknown type: " + id.name()); }
													return Box(*t);
												});
							
							fimp = (*fn)->get_best_fimp(args);
						}
					} else {
						auto &fi((*fn)->get_fimp());
						if (!fi->imp) { fi->compile(form.pos); }
						env.emit(ops::Funcall::type, form.pos, fi);
					}
				}
			}
		}

		Lambda::Lambda(Forms::const_iterator begin, Forms::const_iterator end):
			body(begin, end) { }

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
			auto &op(env.emit(ops::Lambda::type, f.pos).as<ops::Lambda>());
			op.start_pc = env.ops.size();
			env.compile(l.body);

			op.has_vars = (find_if(env.ops.begin() + *op.start_pc, 
														 env.ops.end(), 
														 [](const Op &op) {
															 return &op.type == &ops::GetVar::type ||
															 &op.type == &ops::PutVar::type;
														 }) != env.ops.end());

			env.emit(ops::LambdaRet::type, f.pos);
			op.nops = env.ops.size()-*op.start_pc;
		}
		
		Literal::Literal(const Box &val): val(val) { }

		FormImp *Literal::clone() const { return new Literal(val); }

		void Literal::dump(ostream &out) const { val.dump(out); }

		void Literal::compile(Forms::const_iterator &in,
													Forms::const_iterator end,
													FuncPtr &func, FimpPtr &fimp,
													Env &env) const {
			auto &form(*in++);
			env.emit(ops::Push::type, form.pos, form.as<Literal>().val);			
		}
		
		Semicolon::Semicolon(Forms::const_iterator begin, Forms::const_iterator end):
			body(begin, end) { }

		FormImp *Semicolon::clone() const {
			return new Semicolon(body.begin(), body.end());
		}

		void Semicolon::dump(ostream &out) const {
			out << "; ";
			char sep(0);

			for (auto &f: body) {
				if (sep) { out << sep; }
				f.imp->dump(out);
				sep = ' ';
			}
		}		

		void Semicolon::compile(Forms::const_iterator &in, Forms::const_iterator end,
														FuncPtr &func, FimpPtr &fimp,
														Env &env) const {
			auto &form(*in++);
			if (!func) { throw SyntaxError(form.pos, "Missing func"); }
			env.emit(form.pos, func, fimp);
			env.compile(form.as<Semicolon>().body);
		}

		Sexpr::Sexpr(Forms::const_iterator begin, Forms::const_iterator end):
			body(begin, end) { }

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
			auto &sexpr((*in++).as<Sexpr>());
			env.compile(sexpr.body);
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
			throw Error("Stray type list");
		}		
	}
}
