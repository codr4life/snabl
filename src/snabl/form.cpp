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
		const FormType<Id> Id::type("Id");
		const FormType<Literal> Literal::type("Literal");
		const FormType<Sexpr> Sexpr::type("Sexpr");
		const FormType<TypeList> TypeList::type("TypeList");

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
			} else {
				auto &lib(env.lib());
				auto m(lib.get_macro(id));
				
				if (m) {
					m->call(in, end, func, fimp, env);
				} else {
					in++;
					auto fn(lib.get_func(id));
					if (!fn) { throw Error("Unknown id: " + id.name()); }
					
					if (fn->nargs) {
						if (func) { throw Error("Extra func"); }
						func = fn;

						if (in != end && &in->type == &TypeList::type) {
							auto &ids((in++)->as<TypeList>().ids);
							Stack args;
							transform(ids.begin(), ids.end(), back_inserter(args),
												[&lib](Sym id) {
													auto t(lib.get_type(id));
													if (!t) { throw Error("Unknown type: " + id.name()); }
													return Box(t);
												});
							
							fimp = fn->get_best_fimp(args);
						}
					} else {
						auto fi(fn->get_fimp());
						if (!fi->imp) { Fimp::compile(fi, form.pos); }
						env.emit(ops::Funcall::type, form.pos, fi);
					}
				}
			}
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
		
		Sexpr::Sexpr(const Forms &body): body(body) { }

		FormImp *Sexpr::clone() const { return new Sexpr(body); }

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

		void Sexpr::compile(Forms::const_iterator &in,
												Forms::const_iterator end,
												FuncPtr &func, FimpPtr &fimp,
												Env &env) const {
			auto &sexpr((*in++).as<Sexpr>());
			env.compile(sexpr.body);
		}
		
		TypeList::TypeList(const Forms &body) {
			transform(body.begin(), body.end(), back_inserter(ids),
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
		
		void TypeList::compile(Forms::const_iterator &in,
													 Forms::const_iterator end,
													 FuncPtr &func, FimpPtr &fimp,
													 Env &env) const {
			throw Error("Stray type list");
		}		
	}
}
