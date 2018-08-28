#include <algorithm>

#include "snabl/env.hpp"
#include "snabl/form.hpp"

namespace snabl {
	AFormType::AFormType(stdx::string_view id): id(id) { }

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

		Id::Id(Sym sym): sym(sym) { }

		FormImp *Id::clone() const { return new Id(sym); }

		void Id::dump(std::ostream &out) const { out << sym.name(); }
		
		void Id::compile(Forms::const_iterator &in,
										 Forms::const_iterator end,
										 FuncPtr &func, FimpPtr &fimp,
										 Bin &out) const {
			auto &form(*in);
			auto &id(form.as<Id>().sym);

			if (id.name().front() == '@') {
				in++;
				out.emplace_back(ops::GetVar::type,
												 form.pos,
												 out.env.get_sym(id.name().substr(1)));
			} else {
				auto &env(out.env);
				auto &lib(env.lib());
				auto m(lib.get_macro(id));
				
				if (m) {
					m->call(in, end, func, fimp, out);
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
							std::transform(ids.begin(), ids.end(), std::back_inserter(args),
														 [&lib](Sym id) {
															 auto t(lib.get_type(id));
															 if (!t) { throw Error("Unknown type: " + id.name()); }
															 return Box(t);
														 });
							
							fimp = fn->get_best_fimp(args);
						}
					} else {
						auto fi(fn->get_fimp());
						fi->compile(out, form.pos);
						out.emplace_back(ops::Funcall::type, form.pos, fi);
					}
				}
			}
		}

		Literal::Literal(const Box &value): value(value) { }

		FormImp *Literal::clone() const { return new Literal(value); }

		void Literal::dump(std::ostream &out) const { value.dump(out); }

		void Literal::compile(Forms::const_iterator &in,
													Forms::const_iterator end,
													FuncPtr &func, FimpPtr &fimp,
													Bin &out) const {
			auto &form(*in++);
			out.emplace_back(ops::Push::type, form.pos, form.as<Literal>().value);			
		}
		
		Sexpr::Sexpr(const Forms &body): body(body) { }

		FormImp *Sexpr::clone() const { return new Sexpr(body); }

		void Sexpr::dump(std::ostream &out) const {
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
												Bin &out) const {
			auto &sexpr((*in++).as<Sexpr>());
			out.compile(sexpr.body);
		}
		
		TypeList::TypeList(const Forms &body) {
			std::transform(body.begin(), body.end(), std::back_inserter(ids),
										 [](const Form &f) -> Sym { return f.as<Id>().sym; });
		}

		TypeList::TypeList(const Ids &ids): ids(ids) { }

		FormImp *TypeList::clone() const { return new TypeList(ids); }

		void TypeList::dump(std::ostream &out) const {
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
													 Bin &out) const {
			throw Error("Stray type list");
		}		
	}
}
