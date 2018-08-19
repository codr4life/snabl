#include "snabl/env.hpp"
#include "snabl/form.hpp"

namespace snabl {
	namespace forms {
		const IdType id_type;
		const LiteralType literal_type;

		Id::Id(const Sym &id): id(id) { }

		void IdType::compile(Forms::const_iterator &in,
												 const Forms::const_iterator &end,
												 AFuncPtr &func, AFimpPtr &fimp,
												 Bin &out) const {
			auto f((in++)->as<Id>());
			func = out.env.lib()->get_func(f.id);
		}

		void IdType::dump(const Form &form, std::ostream &out) {
			out << form.as<Id>().id.name();
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
