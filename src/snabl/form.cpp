#include "snabl/form.hpp"

namespace snabl {
	namespace forms {
		const IdType id_type;
		const LiteralType literal_type;

		Id::Id(const std::string &name): name(name) { }

		void IdType::dump(const Form &form, std::ostream &out) {
			out << form.as<Id>().name;
		}

		Literal::Literal(const Box &value): value(value) { }

		void LiteralType::dump(const Form &form, std::ostream &out) {
			form.as<Literal>().value.dump(out);
		}		
	}
}
