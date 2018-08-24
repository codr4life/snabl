#ifndef SNABL_FORM_HPP
#define SNABL_FORM_HPP

#include <iostream>

#include <deque>
#include <vector>

#include "snabl/box.hpp"
#include "snabl/fmt.hpp"
#include "snabl/pos.hpp"
#include "snabl/ptrs.hpp"
#include "snabl/sym.hpp"

namespace snabl {
	class Bin;
	class Env;
	class Form;

	using Forms = std::deque<Form>;

	class AFormType {
	public:
		const std::string id;

		AFormType(const std::string &id);

		virtual void compile(Forms::const_iterator &in,
												 const Forms::const_iterator &end,
												 AFuncPtr &func, AFimpPtr &fimp,
												 Bin &out) const=0;
		
		virtual void dump(const Form &form, std::ostream &out) const=0;
	};

	template <typename ImpT>
	class FormType: public AFormType {
	protected:
		FormType(const std::string &id);
	};

	template <typename ImpT>
	FormType<ImpT>::FormType(const std::string &id): AFormType(id) { }
	
	struct FormImp { };
	
	class Form {
	public:
		const AFormType &type;
		const Pos pos;
		
		template <typename ImpT>
		Form(const FormType<ImpT> &type, const Pos &pos);

		template <typename ImpT, typename ArgT1, typename... ArgsT>
		Form(const FormType<ImpT> &type, const Pos &pos, ArgT1 &&arg1, ArgsT &&... args);

		template <typename ImpT>
		ImpT &as() const;
	private:
		std::unique_ptr<FormImp> _imp;
	};
	
	template <typename ImpT>
	Form::Form(const FormType<ImpT> &type, const Pos &pos): type(type), pos(pos) { }

	template <typename ImpT, typename ArgT1, typename... ArgsT>
	Form::Form(const FormType<ImpT> &type,
						 const Pos &pos,
						 ArgT1 &&arg1, ArgsT &&... args):
		type(type),
		pos(pos),
		_imp(new ImpT(std::forward<ArgT1, ArgsT...>(arg1, args...))) { }

	template <typename ImpT>
	ImpT &Form::as() const {
		if (&ImpT::type != &type) {
			throw Error(fmt("Wrong type: %0 (%1)", ImpT::type.id, type.id));
		}
		
		return *static_cast<ImpT *>(_imp.get());
	}

	namespace forms {
		struct Id;
		
		class IdType: public FormType<Id> {
		public:
			IdType();
			
			void compile(Forms::const_iterator &in,
									 const Forms::const_iterator &end,
									 AFuncPtr &func, AFimpPtr &fimp,
									 Bin &out) const override;
			
			void dump(const Form &form, std::ostream &out) const override;
		};

		struct Id: public FormImp {
			static const IdType type;
			const Sym sym;
			Id(const Sym &sym);
		};

		struct Literal;
		
		class LiteralType: public FormType<Literal> {
		public:
			LiteralType();

			void compile(Forms::const_iterator &in,
									 const Forms::const_iterator &end,
									 AFuncPtr &func, AFimpPtr &fimp,
									 Bin &out) const override;
			
			void dump(const Form &form, std::ostream &out) const override;
		};

		struct Literal: public FormImp {			
			static const LiteralType type;
			Box value;
			Literal(const Box &value);
		};

		struct Sexpr;
		
		class SexprType: public FormType<Sexpr> {
		public:
			SexprType();

			void compile(Forms::const_iterator &in,
									 const Forms::const_iterator &end,
									 AFuncPtr &func, AFimpPtr &fimp,
									 Bin &out) const override;
			
			void dump(const Form &form, std::ostream &out) const override;
		};

		struct Sexpr: public FormImp {			
			static const SexprType type;
			const Forms body;
			Sexpr(Forms &&body);
		};

		struct TypeList;
		
		class TypeListType: public FormType<TypeList> {
		public:
			TypeListType();

			void compile(Forms::const_iterator &in,
									 const Forms::const_iterator &end,
									 AFuncPtr &func, AFimpPtr &fimp,
									 Bin &out) const override;
			
			void dump(const Form &form, std::ostream &out) const override;
		};

		struct TypeList: public FormImp {			
			static const TypeListType type;
			std::vector<Sym> ids;
			TypeList(const Forms &body);
		};
	}
}

#endif
