#include <algorithm>

#include "snabl/env.hpp"
#include "snabl/form.hpp"

namespace snabl {
	AFormType::AFormType(const std::string &id): id(id) { }

	namespace forms {
		const IdType Id::type;
		const LiteralType Literal::type;
		const SexprType Sexpr::type;
		const TypeListType TypeList::type;

		Id::Id(const Sym &sym): sym(sym) { }

		IdType::IdType(): FormType<Id>("Id") { }
		
		void IdType::compile(Forms::const_iterator &in,
												 const Forms::const_iterator &end,
												 AFuncPtr &func, AFimpPtr &fimp,
												 Bin &out) const {
			auto &op(*in);
			auto &id(op.as<Id>().sym);

			if (id.name().front() == '@') {
				in++;
				out.emplace_back(ops::GetVar::type,
												 op.pos,
												 out.env.get_sym(id.name().substr(1)));
			} else {
				auto &env(out.env);
				auto &lib(env.lib());
				auto lc(id.name().back());
				
				if (lc == ':' || lc == '!') {
					auto m(lib.get_macro(id));
					if (!m) { throw Error("Unknown macro: " + id.name()); }					
					m->call(in, end, func, fimp, out);
				} else {
					in++;
					auto fn(lib.get_func(id));
					if (!fn) { throw Error("Unknown func: " + id.name()); }
					
					if (fn->nargs) {
						if (func) { throw Error("Extra func"); }
						func = fn;

						if (in != end && &in->type == &TypeList::type) {
							auto &ids((in++)->as<TypeList>().ids);
							Stack args;
							std::transform(ids.begin(), ids.end(), std::back_inserter(args),
														 [&lib](const Sym &id) {
															 auto t(lib.get_type(id));
															 if (!t) { throw Error("Unknown type: " + id.name()); }
															 return Box(t);
														 });
							
							fimp = fn->get_best_fimp(args);
						}
					} else {
						out.emplace_back(ops::Funcall::type, op.pos, fn->get_fimp());
					}
				}
			}
		}

		void IdType::dump(const Form &form, std::ostream &out) const {
			out << form.as<Id>().sym.name();
		}

		Literal::Literal(const Box &value): value(value) { }
		
		LiteralType::LiteralType(): FormType<Literal>("Literal") { }

		void LiteralType::dump(const Form &form, std::ostream &out) const {
			form.as<Literal>().value.dump(out);
		}		

		void LiteralType::compile(Forms::const_iterator &in,
															const Forms::const_iterator &end,
															AFuncPtr &func, AFimpPtr &fimp,
															Bin &out) const {
			auto &op(*in++);
			auto &f(op.as<Literal>());
			out.emplace_back(ops::Push::type, op.pos, f.value);			
		}

		Sexpr::Sexpr(Forms &&body): body(std::move(body)) { }

		SexprType::SexprType(): FormType<Sexpr>("Sexpr") { }
		
		void SexprType::dump(const Form &form, std::ostream &out) const {
			out << '(';
			auto &op(form.as<Sexpr>());
			char sep(0);

			for (auto &f: op.body) {
				if (sep) { out << sep; }
				f.type.dump(f, out);
				sep = ' ';
			}
			
			out << ')';
		}		

		void SexprType::compile(Forms::const_iterator &in,
														const Forms::const_iterator &end,
														AFuncPtr &func, AFimpPtr &fimp,
														Bin &out) const {
			auto &op((*in++).as<Sexpr>());
			out.compile(op.body);
		}

		TypeList::TypeList(const Forms &body) {
			std::transform(body.begin(), body.end(), std::back_inserter(ids),
										 [](const Form &f) -> Sym { return f.as<Id>().sym; });
		}

		TypeListType::TypeListType(): FormType<TypeList>("TypeList") { }

		void TypeListType::dump(const Form &form, std::ostream &out) const {
			out << '<';
			auto &op(form.as<TypeList>());
			char sep(0);

			for (auto &id: op.ids) {
				if (sep) { out << sep; }
				out << id.name();
				sep = ' ';
			}
			
			out << '>';
		}		

		void TypeListType::compile(Forms::const_iterator &in,
														const Forms::const_iterator &end,
														AFuncPtr &func, AFimpPtr &fimp,
														Bin &out) const {
			throw Error("Stray type list");
		}
	}
}
