#ifndef SNABL_FORM_HPP
#define SNABL_FORM_HPP

#include "snabl/box.hpp"

namespace snabl {
	class Form;
	
	class AFormType {
	public:
		virtual void dump(const Form &form, std::ostream &out)=0;
	};

	template <typename ImpT>
	class FormType: public AFormType { };

	struct FormImp { };
	
	class Form {
	public:
		const AFormType &type;
		
		template <typename ImpT, typename... ArgsT>
		Form(const FormType<ImpT> &type, ArgsT... args);

		template <typename ImpT>
		ImpT &as() const;
	private:
		std::unique_ptr<FormImp> _imp;
	};

	template <typename ImpT, typename... ArgsT>
	Form::Form(const FormType<ImpT> &type, ArgsT... args):
		type(type), _imp(new ImpT(args...)) { }

	template <typename ImpT>
	ImpT &Form::as() const { return *static_cast<ImpT *>(_imp.get()); }

	namespace forms {
		struct Id: public FormImp {
			const std::string name;
			Id(const std::string &name);
		};

		class IdType: public FormType<Id> {
		public:
			virtual void dump(const Form &form, std::ostream &out);
		};

		extern const IdType id_type;

		struct Literal: public FormImp {			
			Box value;
			Literal(const Box &value);
		};

		class LiteralType: public FormType<Literal> {
		public:
			virtual void dump(const Form &form, std::ostream &out);
		};

		extern const LiteralType literal_type;
	}
}

#endif
