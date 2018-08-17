#include "snabl/form.hpp"

namespace snabl {
	FormType::FormType() { }

	namespace forms {
		void IdType::dump(const Form &form, std::ostream &out) {
			out << form.as_id.name;
		}

		const IdType Id::type;

		Id::Id(const std::string &name): name(name) { }

		void LiteralType::dump(const Form &form, std::ostream &out) {
			form.as_literal.value.dump(out);
		}

		const LiteralType Literal::type;

		Literal::Literal(const Box &value): value(value) { }
	}

	Form::Form(const FormType &type): type(type) { }

	Form::~Form() {}
}
