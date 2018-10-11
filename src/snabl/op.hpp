#ifndef SNABL_OP_HPP
#define SNABL_OP_HPP

#include "snabl/box.hpp"
#include "snabl/pos.hpp"
#include "snabl/ptrs.hpp"
#include "snabl/scope.hpp"
#include "snabl/state.hpp"
#include "snabl/std.hpp"
#include "snabl/sym.hpp"
#include "snabl/target.hpp"
#include "snabl/type.hpp"
#include "snabl/types.hpp"

namespace snabl {    
  struct Fimp;
  struct Func;
  
  struct Op {
    const Sym type;
    const Pos pos;
    PC next;

    Op(const Op &)=delete;
    const Op &operator =(const Op &)=delete;

    Op(Env &env, const string &type, Pos pos);
    virtual ~Op() { }
    virtual void run(Env &env)=0;
    virtual void dump_args(ostream &out) const {}
    void dump(ostream &out) const;
  };

  using Ops = deque<unique_ptr<Op>>;

  namespace ops {    
    struct Bench: Op {
      static const string type;
      I64 end_pc;
      Bench(Env &env, Pos pos);
      void run(Env &env) override;
    };

    struct Call: Op {       
      static const string type;
      Call(Env &env, Pos pos);
      void run(Env &env) override;
    };

    struct DDrop: Op {
      static const string type;
      DDrop(Env &env, Pos pos);
      void run(Env &env) override;
    };

    struct Drop: Op {
      static const string type;
      Drop(Env &env, Pos pos);
      void run(Env &env) override;
    };

    struct Dup: Op {
      static const string type;
      Dup(Env &env, Pos pos);
      void run(Env &env) override;
    };

    struct Else: Op {
      static const string type;
      I64 skip_pc;
      Else(Env &env, Pos pos, I64 skip_pc=-1);
      void run(Env &env) override;
    };
    
    struct Eqval: Op {
      static const string type;
      const optional<const Box> rhs;
      Eqval(Env &env, Pos pos, const optional<const Box> rhs=nullopt);
      void run(Env &env) override;
      void dump_args(ostream &out) const override;
    };

    struct Fimp: Op {
      static const string type;
      snabl::Fimp &fimp;
      const bool is_scope;
      Fimp(Env &env, Pos pos, snabl::Fimp &fimp, bool is_scope);
      void run(Env &env) override;
      void dump_args(ostream &out) const override;
    };

    struct Funcall: Op {
      static const string type;
      Func &func;
      snabl::Fimp *const fimp, *prev_fimp;
      Funcall(Env &env, Pos pos, Func &func);
      Funcall(Env &env, Pos pos, snabl::Fimp &fimp);
      void run(Env &env) override;
      void dump_args(ostream &out) const override;
  };
    
    struct Get: Op {
      static const string type;
      const Sym id;
      Get(Env &env, Pos pos, Sym id);
      void run(Env &env) override;
    };

    struct Isa: Op {
      static const string type;
      const AType &rhs;
      Isa(Env &env, Pos pos, const AType &rhs);
      void run(Env &env) override;
      void dump_args(ostream &out) const override;
    };

    struct Jump: Op {
      static const string type;
      I64 end_pc;
      Jump(Env &env, Pos pos, I64 end_pc=-1);
      void run(Env &env) override;
    };

    struct JumpIf: Op {
      static const string type;
      function<bool ()> cond;
      I64 end_pc;
      JumpIf(Env &env, Pos pos, function<bool ()> &&cond);
      void run(Env &env) override;
    };

    struct Lambda: Op {
      static const string type;
      const bool is_scope;
      PC start_pc;
      I64 end_pc;
      Lambda(Env &env, Pos pos, bool is_scope);
      void run(Env &env) override;
    };

    struct Let: Op {
      static const string type;
      const Sym id;
      Let(Env &env, Pos pos, Sym id);
      void run(Env &env) override;
      void dump_args(ostream &out) const override;
    };

    struct Nop: Op {
      static const string type;
      Nop(Env &env, Pos pos);
      void run(Env &env) override;
    };

    struct Push: Op {
      static const string type;     
      const Box val;
      Push(Env &env, Pos pos, const Box &val);
      
      template <typename ValT, typename... ArgsT>
      Push(Env &env, Pos pos, Type<ValT> &type, ArgsT &&...args):
        Op(env, Push::type, pos), val(type, forward<ArgsT>(args)...) { }

      void run(Env &env) override;
      void dump_args(ostream &out) const override;
    };

    struct Recall: Op {
      static const string type;
      Recall(Env &env, Pos pos);
      void run(Env &env) override;
    };

    struct Return: Op {
      static const string type;
      Return(Env &env, Pos pos);
      void run(Env &env) override;
    };

    struct Rot: Op {
      static const string type;
      Rot(Env &env, Pos pos);
      void run(Env &env) override;
    };

    struct RSwap: Op {
      static const string type;
      RSwap(Env &env, Pos pos);
      void run(Env &env) override;
    };

    struct Scope: Op {
      static const string type;
      Scope(Env &env, Pos pos);
      void run(Env &env) override;
    };

    struct ScopeEnd: Op {
      static const string type;
      ScopeEnd(Env &env, Pos pos);
      void run(Env &env) override;
    };

    struct SDrop: Op {
      static const string type;
      SDrop(Env &env, Pos pos);
      void run(Env &env) override;
    };

    struct Split: Op {
      static const string type;
      const I64 offs;
      Split(Env &env, Pos pos, I64 offs=0);
      void run(Env &env) override;
    };

    struct SplitEnd: Op {
      static const string type;
      SplitEnd(Env &env, Pos pos);
      void run(Env &env) override;
    };

    struct Stack: Op {
      static const string type;
      const bool end_split;
      Stack(Env &env, Pos pos, bool end_split);
      void run(Env &env) override;
    };

    struct Stop: Op {
      static const string type;
      Stop(Env &env, Pos pos);
      void run(Env &env) override;
    };

    struct Swap: Op {
      static const string type;
      Swap(Env &env, Pos pos);
      void run(Env &env) override;
    };

    struct Sync: Op {
      static const string type;
      Sync(Env &env, Pos pos);
      void run(Env &env) override;
    };

    struct Task: Op {
      static const string type;
      PC start_pc;
      I64 end_pc;
      Task(Env &env, Pos pos);
      void run(Env &env) override;
    };

    struct Throw: Op {
      static const string type;
      Throw(Env &env, Pos pos);
      void run(Env &env) override;
    };
    
    struct Times: Op {
      static const string type;
      const I64 i_reg;
      Times(Env &env, Pos pos, I64 i_reg);
      void run(Env &env) override;
    };

    struct Try: Op {
      static const string type;
      const I64 state_reg;
      I64 end_pc;
      Try(Env &env, Pos pos, I64 state_reg);
      void run(Env &env) override;
    };

    struct TryEnd: Op {
      static const string type;
      const I64 state_reg;
      TryEnd(Env &env, Pos pos, I64 state_reg);
      void run(Env &env) override;
    };

    struct Yield: Op {
      static const string type;
      Yield(Env &env, Pos pos);
      void run(Env &env) override;
    };
  } 
}

#endif
