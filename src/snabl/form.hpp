#ifndef SNABL_FORM_HPP
#define SNABL_FORM_HPP

#include "snabl/box.hpp"
#include "snabl/fmt.hpp"
#include "snabl/pos.hpp"
#include "snabl/ptrs.hpp"
#include "snabl/std.hpp"
#include "snabl/sym.hpp"

namespace snabl {
	class Env;
	class Form;

	using Forms = deque<Form>;

	struct AFormType {
		const string id;
		AFormType(string_view id);		
	};

	template <typename ImpT>
	struct FormType: public AFormType {
		FormType(string_view id);
	};

	template <typename ImpT>
	FormType<ImpT>::FormType(string_view id): AFormType(id) { }
	
	struct FormImp {
		virtual ~FormImp();
		virtual FormImp *clone() const=0;
		virtual void dump(ostream &out) const=0;

		virtual void compile(Forms::const_iterator &in,
												 Forms::const_iterator end,
												 FuncPtr &func, FimpPtr &fimp,
												 Env &env) const=0;
	};
	
	class Form {
	public:
		const AFormType &type;
		const Pos pos;
		const unique_ptr<FormImp> imp;
		
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
		imp(new ImpT(forward<ArgT1, ArgsT...>(arg1, args...))) { }

	template <typename ImpT>
	ImpT &Form::as() const {
		if (&ImpT::type != &type) {
			throw Error(fmt("Wrong type: %0 (%1)", {ImpT::type.id, type.id}));
		}
		
		return *static_cast<ImpT *>(imp.get());
	}

	namespace forms {
		struct Id: public FormImp {
			static const FormType<Id> type;
			const Sym id;
			
			Id(Sym id);
			FormImp *clone() const override;
			void dump(ostream &out) const override;

			void compile(Forms::const_iterator &in,
									 Forms::const_iterator end,
									 FuncPtr &func, FimpPtr &fimp,
									 Env &env) const override;
		};

		struct Literal: public FormImp {			
			static const FormType<Literal> type;
			const Box val;
			Literal(const Box &val);
			FormImp *clone() const override;
			void dump(ostream &out) const override;

			void compile(Forms::const_iterator &in,
									 Forms::const_iterator end,
									 FuncPtr &func, FimpPtr &fimp,
									 Env &env) const override;
		};

		struct Sexpr: public FormImp {			
			static const FormType<Sexpr> type;
			const Forms body;
			Sexpr(const Forms &body);
			FormImp *clone() const override;
			void dump(ostream &out) const override;

			void compile(Forms::const_iterator &in,
									 Forms::const_iterator end,
									 FuncPtr &func, FimpPtr &fimp,
									 Env &env) const override;
		};

		struct TypeList: public FormImp {
			using Ids = vector<Sym>;
				
			static const FormType<TypeList> type;
			Ids ids;
			
			TypeList(const Forms &body);
			TypeList(const Ids &ids);
			FormImp *clone() const override;
			void dump(ostream &out) const override;

			void compile(Forms::const_iterator &in,
									 Forms::const_iterator end,
									 FuncPtr &func, FimpPtr &fimp,
									 Env &env) const override;			
		};
	}
}

#endif
