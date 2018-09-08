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
		AFormType(const AFormType &) = delete;
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
		
		template <typename ImpT, typename... ArgsT>
		Form(const FormType<ImpT> &type, Pos pos, ArgsT &&... args);

		Form(const Form &source);
		
		virtual ~Form() { }
		template <typename ImpT>
		ImpT &as() const;
	};
	
	template <typename ImpT, typename... ArgsT>
	Form::Form(const FormType<ImpT> &type, Pos pos, ArgsT &&... args):
		type(type), pos(pos), imp(new ImpT(forward<ArgsT>(args)...)) { }

	template <typename ImpT>
	ImpT &Form::as() const {
		if (&ImpT::type != &type) {
			throw Error(fmt("Wrong type: %0 (%1)", {ImpT::type.id, type.id}));
		}
		
		return *dynamic_cast<ImpT *>(imp.get());
	}

	namespace forms {
		struct Comma: public FormImp {			
			static const FormType<Comma> type;
			const Forms body;

			Comma(Forms::const_iterator begin, Forms::const_iterator end);
			FormImp *clone() const override;
			void dump(ostream &out) const override;

			void compile(Forms::const_iterator &in,
									 Forms::const_iterator end,
									 FuncPtr &func, FimpPtr &fimp,
									 Env &env) const override;
		};

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

		struct Lambda: public FormImp {			
			static const FormType<Lambda> type;
			const Forms body;

			Lambda(Forms::const_iterator begin, Forms::const_iterator end);
			FormImp *clone() const override;
			void dump(ostream &out) const override;

			void compile(Forms::const_iterator &in,
									 Forms::const_iterator end,
									 FuncPtr &func, FimpPtr &fimp,
									 Env &env) const override;
		};

		struct Lit: public FormImp {			
			static const FormType<Lit> type;
			const Box val;

			Lit(const Box &val);
			FormImp *clone() const override;
			void dump(ostream &out) const override;

			void compile(Forms::const_iterator &in,
									 Forms::const_iterator end,
									 FuncPtr &func, FimpPtr &fimp,
									 Env &env) const override;
		};

		struct Query: public FormImp {			
			static const FormType<Query> type;
			const Form form;
			
			Query(const Form &form);
			FormImp *clone() const override;
			void dump(ostream &out) const override;

			void compile(Forms::const_iterator &in,
									 Forms::const_iterator end,
									 FuncPtr &func, FimpPtr &fimp,
									 Env &env) const override;
		};

		struct Semi: public FormImp {			
			static const FormType<Semi> type;
			const Forms body;

			Semi(Forms::const_iterator begin, Forms::const_iterator end);
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

			Sexpr(Forms::const_iterator begin, Forms::const_iterator end);
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
			
			TypeList(Forms::const_iterator begin, Forms::const_iterator end);
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
