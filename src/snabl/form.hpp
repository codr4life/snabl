#ifndef SNABL_FORM_HPP
#define SNABL_FORM_HPP

#include <deque>

#include "snabl/box.hpp"

namespace snabl {
	class Bin;
	class Env;
	class Form;

	using Forms = std::deque<Form>;

	class AFormType {
	public:
		virtual void compile(Forms::const_iterator &in,
												 const Forms::const_iterator &end,
												 Bin &out) const=0;
		
		virtual void dump(const Form &form, std::ostream &out)=0;
	};

	template <typename ImpT>
	class FormType: public AFormType { };

	struct FormImp { };
	
	class Form {
	public:
		const AFormType &type;
		
		template <typename ImpT>
		Form(const FormType<ImpT> &type, ImpT *imp);

		template <typename ImpT, typename ArgT1, typename... ArgsT>
		Form(const FormType<ImpT> &type, ArgT1 &&arg1, ArgsT &&... args);

		template <typename ImpT>
		ImpT &as() const;
	private:
		std::unique_ptr<FormImp> _imp;
	};
	
	template <typename ImpT>
	Form::Form(const FormType<ImpT> &type, ImpT *imp): type(type), _imp(imp) { }

	template <typename ImpT, typename ArgT1, typename... ArgsT>
	Form::Form(const FormType<ImpT> &type, ArgT1 &&arg1, ArgsT &&... args):
		type(type), _imp(new ImpT(arg1, std::forward<ArgT1, ArgsT...>(arg1, args...))) { }

	template <typename ImpT>
	ImpT &Form::as() const { return *static_cast<ImpT *>(_imp.get()); }

	namespace forms {
		struct Id: public FormImp {
			const Sym id;
			Id(const Sym &id);
		};

		class IdType: public FormType<Id> {
		public:
			void compile(Forms::const_iterator &in,
									 const Forms::const_iterator &end,
									 Bin &out) const override;
			
			void dump(const Form &form, std::ostream &out) override;
		};

		extern const IdType id_type;

		struct Literal: public FormImp {			
			Box value;
			Literal(const Box &value);
		};

		class LiteralType: public FormType<Literal> {
		public:
			void compile(Forms::const_iterator &in,
									 const Forms::const_iterator &end,
									 Bin &out) const override;
			
			void dump(const Form &form, std::ostream &out) override;
		};

		extern const LiteralType literal_type;
	}
}

#endif
