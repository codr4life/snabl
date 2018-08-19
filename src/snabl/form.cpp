#include "snabl/env.hpp"
#include "snabl/form.hpp"

namespace snabl {
	namespace forms {
		const IdType Id::type;
		const LiteralType Literal::type;

		Id::Id(const Sym &sym): sym(sym) { }

		void IdType::compile(Forms::const_iterator &in,
												 const Forms::const_iterator &end,
												 AFuncPtr &func, AFimpPtr &fimp,
												 Bin &out) const {
			auto &id((in++)->as<Id>().sym);
			auto &lib(out.env.lib());
			auto m(lib.get_macro(id));
			
			if (m) {
				m->call(in, end, func, fimp, out);
			} else {
				auto f(lib.get_func(id));
				if (!f) { throw new Error("Unknown id"); }
				
				if (f->nargs) {
					if (func) { throw new Error("Extra func"); }
					func = f;
				} else {
					out.emplace_back(ops::Funcall::type, f);
				}
			}
		}

		void IdType::dump(const Form &form, std::ostream &out) {
			out << form.as<Id>().sym.name();
		}

		Literal::Literal(const Box &value): value(value) { }

		void LiteralType::dump(const Form &form, std::ostream &out) {
			form.as<Literal>().value.dump(out);
		}		

		void LiteralType::compile(Forms::const_iterator &in,
															const Forms::const_iterator &end,
															AFuncPtr &func, AFimpPtr &fimp,
															Bin &out) const {
			auto f((in++)->as<Literal>());
			out.emplace_back(ops::Push::type, f.value);			
		}
	}
}
