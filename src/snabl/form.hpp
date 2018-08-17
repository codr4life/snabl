#ifndef SNABL_FORM_HPP
#define SNABL_FORM_HPP

#include "snabl/box.hpp"

namespace snabl {
	class Form;
	
	class FormType {
	public:
		FormType();
		virtual void dump(const Form &form, std::ostream &out)=0;
	};

	namespace forms {
		class IdType: public FormType {
		public:
			virtual void dump(const Form &form, std::ostream &out);
		};

		struct Id {
			static const IdType type;
			const std::string name;
			Id(const std::string &name);
		};

		class LiteralType: public FormType {
		public:
			virtual void dump(const Form &form, std::ostream &out);
		};

		struct Literal {
			static const LiteralType type;
			
			Box value;
			Literal(const Box &value);
		};
	}

	class Form {
	public:
		const FormType &type;

		union {
			forms::Id as_id;
			forms::Literal as_literal;
		}; 

		Form(const FormType &type);
		~Form();
	};
}

#endif
