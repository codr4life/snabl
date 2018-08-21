#include "snabl/env.hpp"
#include "snabl/form.hpp"

namespace snabl {
	namespace forms {
		const IdType Id::type;
		const LiteralType Literal::type;
		const SexprType Sexpr::type;

		Id::Id(const Sym &sym): sym(sym) { }

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
				auto &lib(out.env.lib());
				auto m(lib.get_macro(id));
				
				if (m) {
					m->call(in, end, func, fimp, out);
				} else {
					in++;
					auto f(lib.get_func(id));
					if (!f) { throw Error("Unknown id"); }
					
					if (f->nargs) {
						if (func) { throw Error("Extra func"); }
						func = f;
					} else {
						out.emplace_back(ops::Funcall::type, op.pos, f);
					}
				}
			}
		}

		void IdType::dump(const Form &form, std::ostream &out) const {
			out << form.as<Id>().sym.name();
		}

		Literal::Literal(const Box &value): value(value) { }

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
	}
}
