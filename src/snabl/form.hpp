#ifndef SNABL_FORM_HPP
#define SNABL_FORM_HPP

#include <iostream>

#include <deque>
#include <vector>

#include "snabl/box.hpp"
#include "snabl/fmt.hpp"
#include "snabl/pos.hpp"
#include "snabl/ptrs.hpp"
#include "snabl/std/string_view.hpp"
#include "snabl/sym.hpp"

namespace snabl {
	class Bin;
	class Env;
	class Form;

	using Forms = std::deque<Form>;

	struct AFormType {
		const std::string id;
		AFormType(std::string_view id);		
	};

	template <typename ImpT>
	struct FormType: public AFormType {
		FormType(std::string_view id);
	};

	template <typename ImpT>
	FormType<ImpT>::FormType(std::string_view id): AFormType(id) { }
	
	struct FormImp {
		virtual ~FormImp();
		virtual FormImp *clone() const=0;
		virtual void dump(std::ostream &out) const=0;

		virtual void compile(Forms::const_iterator &in,
												 Forms::const_iterator end,
												 FuncPtr &func, FimpPtr &fimp,
												 Bin &out) const=0;
	};
	
	class Form {
	public:
		const AFormType &type;
		const Pos pos;
		const std::unique_ptr<FormImp> imp;
		
		template <typename ImpT>
		Form(const FormType<ImpT> &type, Pos pos);

		template <typename ImpT, typename ArgT1, typename... ArgsT>
		Form(const FormType<ImpT> &type, Pos pos, ArgT1 &&arg1, ArgsT &&... args);

		Form(const Form &source);
		
		virtual ~Form() { }
		template <typename ImpT>
		ImpT &as() const;
	};
	
	template <typename ImpT>
	Form::Form(const FormType<ImpT> &type, Pos pos): type(type), pos(pos) { }

	template <typename ImpT, typename ArgT1, typename... ArgsT>
	Form::Form(const FormType<ImpT> &type,
						 Pos pos,
						 ArgT1 &&arg1, ArgsT &&... args):
		type(type),
		pos(pos),
		imp(new ImpT(std::forward<ArgT1, ArgsT...>(arg1, args...))) { }

	template <typename ImpT>
	ImpT &Form::as() const {
		if (&ImpT::type != &type) {
			throw Error(fmt("Wrong type: %0 (%1)", ImpT::type.id, type.id));
		}
		
		return *static_cast<ImpT *>(imp.get());
	}

	namespace forms {
		struct Id: public FormImp {
			static const FormType<Id> type;
			const Sym sym;
			Id(Sym sym);
			FormImp *clone() const override;
			void dump(std::ostream &out) const override;

			void compile(Forms::const_iterator &in,
									 Forms::const_iterator end,
									 FuncPtr &func, FimpPtr &fimp,
									 Bin &out) const override;
		};

		struct Literal: public FormImp {			
			static const FormType<Literal> type;
			Box value;
			Literal(const Box &value);
			FormImp *clone() const override;
			void dump(std::ostream &out) const override;

			void compile(Forms::const_iterator &in,
									 Forms::const_iterator end,
									 FuncPtr &func, FimpPtr &fimp,
									 Bin &out) const override;
		};

		struct Sexpr: public FormImp {			
			static const FormType<Sexpr> type;
			const Forms body;
			Sexpr(const Forms &body);
			FormImp *clone() const override;
			void dump(std::ostream &out) const override;

			void compile(Forms::const_iterator &in,
									 Forms::const_iterator end,
									 FuncPtr &func, FimpPtr &fimp,
									 Bin &out) const override;
		};

		struct TypeList: public FormImp {
			using Ids = std::vector<Sym>;
				
			static const FormType<TypeList> type;
			Ids ids;
			
			TypeList(const Forms &body);
			TypeList(const Ids &ids);
			FormImp *clone() const override;
			void dump(std::ostream &out) const override;

			void compile(Forms::const_iterator &in,
									 Forms::const_iterator end,
									 FuncPtr &func, FimpPtr &fimp,
									 Bin &out) const override;			
		};
	}
}

#endif
