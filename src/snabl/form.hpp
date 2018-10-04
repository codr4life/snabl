#ifndef SNABL_FORM_HPP
#define SNABL_FORM_HPP

#include "snabl/box.hpp"
#include "snabl/fmt.hpp"
#include "snabl/pos.hpp"
#include "snabl/ptrs.hpp"
#include "snabl/std.hpp"
#include "snabl/sym.hpp"

namespace snabl {
  struct Env;
  struct Form;

  using Forms = deque<Form>;

  struct AFormType {
    const string id;
    AFormType(string_view id);
    AFormType(const AFormType &) = delete;
  };

  template <typename ImpT>
  struct FormType: AFormType {
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
                         Env &env) const=0;
  };
  
  struct Form {
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
    return *dynamic_cast<ImpT *>(imp.get());
  }

  namespace forms {
    struct Body: FormImp {     
      const Forms body;
      Body(Forms::const_iterator begin, Forms::const_iterator end):
        body(begin, end) { }
      void dump(ostream &out) const override;
    };

    struct Fimp: FormImp {
      using Ids = vector<Sym>;
        
      static const FormType<Fimp> type;
      const Sym id;
      Ids type_ids;
      
      Fimp(Sym id, Forms::const_iterator begin, Forms::const_iterator end);
      Fimp(Sym id, const Ids &type_ids);
      FormImp *clone() const override;
      void dump(ostream &out) const override;

      void compile(Forms::const_iterator &in,
                   Forms::const_iterator end,
                   Env &env) const override;      
    };
    
    struct Id: FormImp {
      static const FormType<Id> type;
      const Sym id;
      
      Id(Sym id);
      FormImp *clone() const override;
      void dump(ostream &out) const override;

      void compile(Forms::const_iterator &in,
                   Forms::const_iterator end,
                   Env &env) const override;
    };

    struct Lit: FormImp {      
      static const FormType<Lit> type;
      const Box val;

      Lit(const Box &val);
      FormImp *clone() const override;
      void dump(ostream &out) const override;

      void compile(Forms::const_iterator &in,
                   Forms::const_iterator end,
                   Env &env) const override;
    };

    struct More: Body {      
      static const FormType<More> type;

      More(Forms::const_iterator begin, Forms::const_iterator end):
        Body(begin, end) { }
      
      FormImp *clone() const override;
      void dump(ostream &out) const override;

      void compile(Forms::const_iterator &in,
                   Forms::const_iterator end,
                   Env &env) const override;
    };

    struct Query: FormImp {      
      static const FormType<Query> type;
      const Form form;
      
      Query(const Form &form);
      FormImp *clone() const override;
      void dump(ostream &out) const override;

      void compile(Forms::const_iterator &in,
                   Forms::const_iterator end,
                   Env &env) const override;
    };

    struct Ref: FormImp {      
      static const FormType<Ref> type;
      const Form form;
      
      Ref(const Form &form);
      FormImp *clone() const override;
      void dump(ostream &out) const override;

      void compile(Forms::const_iterator &in,
                   Forms::const_iterator end,
                   Env &env) const override;
    };

    struct Scope: Body {     
      static const FormType<Scope> type;

      Scope(Forms::const_iterator begin, Forms::const_iterator end):
        Body(begin, end) { }
      
      FormImp *clone() const override;
      void dump(ostream &out) const override;

      void compile(Forms::const_iterator &in,
                   Forms::const_iterator end,
                   Env &env) const override;
    };

    struct Sexpr: Body {     
      static const FormType<Sexpr> type;

      Sexpr(Forms::const_iterator begin, Forms::const_iterator end):
        Body(begin, end) { }
      
      FormImp *clone() const override;
      void dump(ostream &out) const override;

      void compile(Forms::const_iterator &in,
                   Forms::const_iterator end,
                   Env &env) const override;
    };

    struct Split: Body {     
      static const FormType<Split> type;
      Int offs=0;
      
      Split(Forms::const_iterator begin, Forms::const_iterator end):
        Body(begin, end) { }
      
      FormImp *clone() const override;
      void dump(ostream &out) const override;

      void compile(Forms::const_iterator &in,
                   Forms::const_iterator end,
                   Env &env) const override;
    };

    struct Stack: Body {     
      static const FormType<Stack> type;

      Stack(Forms::const_iterator begin, Forms::const_iterator end):
        Body(begin, end) { }
      
      FormImp *clone() const override;
      void dump(ostream &out) const override;

      void compile(Forms::const_iterator &in,
                   Forms::const_iterator end,
                   Env &env) const override;
    };
  }
}

#endif
