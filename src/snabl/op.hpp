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
  struct Op;
  
  using Ops = deque<unique_ptr<Op>>;
  
  struct OpType {
    const string id;

    OpType(const OpType &) = delete;
    const OpType &operator=(const OpType &) = delete;

    OpType(const string &id): id(id) { }   
  };
    
  struct Op {
    const OpType &type;
    const Pos pos;
    const OpImp imp;
    PC next;

    Op(const Op &)=delete;
    const Op &operator =(const Op &)=delete;

    Op(const OpType &type, Pos pos, OpImp imp):
      type(type), pos(pos), imp(imp), next(nullptr) { }

    virtual ~Op() { }
    virtual void dump_args(ostream &out) const {}

    void dump(ostream &out) const {
      out << type.id;
      dump_args(out);
      out << endl;
    }
  };
  
  namespace ops {
    struct Bench: Op {
      static const OpType type;
      Int end_pc;
      Bench(Env &env, Pos pos);
      OpImp make_imp(Env &env);
    };

    struct Call: Op {       
      static const OpType type;
      Call(Env &env, Pos pos);
      OpImp make_imp(Env &env);
    };

    struct DDrop: Op {
      static const OpType type;
      DDrop(Env &env, Pos pos);
      OpImp make_imp(Env &env);
    };

    struct Drop: Op {
      static const OpType type;
      Drop(Env &env, Pos pos);
      OpImp make_imp(Env &env);
    };

    struct Dup: Op {
      static const OpType type;
      Dup(Env &env, Pos pos);
      OpImp make_imp(Env &env);
    };

    struct Else: Op {
      static const OpType type;
      Int skip_pc;
      Else(Env &env, Pos pos, Int skip_pc=-1);
      OpImp make_imp(Env &env);
    };
    
    struct Eqval: Op {
      static const OpType type;
      const optional<const Box> rhs;
      Eqval(Env &env, Pos pos, const optional<const Box> rhs=nullopt);
      OpImp make_imp(Env &env);
      void dump_args(ostream &out) const override;
    };

    struct Fimp: Op {
      static const OpType type;
      snabl::Fimp &fimp;
      const bool is_scope;
      Fimp(Env &env, Pos pos, snabl::Fimp &fimp, bool is_scope);
      OpImp make_imp(Env &env);
      void dump_args(ostream &out) const override;
    };

    struct Funcall: Op {
      static const OpType type;
      Func &func;
      snabl::Fimp *const fimp, *prev_fimp;
      Funcall(Env &env, Pos pos, Func &func);
      Funcall(Env &env, Pos pos, snabl::Fimp &fimp);
      OpImp make_imp(Env &env);
      void dump_args(ostream &out) const override;
  };
    
    struct Get: Op {
      static const OpType type;
      const Sym id;
      Get(Env &env, Pos pos, Sym id);
      OpImp make_imp(Env &env);
    };

    struct Isa: Op {
      static const OpType type;
      const AType &rhs;
      Isa(Env &env, Pos pos, const AType &rhs);
      OpImp make_imp(Env &env);
      void dump_args(ostream &out) const override;
    };

    struct Jump: Op {
      static const OpType type;
      Int end_pc;
      Jump(Env &env, Pos pos, Int end_pc=-1);
      OpImp make_imp(Env &env);
    };

    struct JumpIf: Op {
      static const OpType type;
      function<bool ()> cond;
      Int end_pc;
      JumpIf(Env &env, Pos pos, function<bool ()> &&cond);
      OpImp make_imp(Env &env);
    };

    struct Lambda: Op {
      static const OpType type;
      const bool is_scope;
      PC start_pc;
      Int end_pc;
      Lambda(Env &env, Pos pos, bool is_scope);
      OpImp make_imp(Env &env);
    };

    struct Let: Op {
      static const OpType type;
      const Sym id;
      Let(Env &env, Pos pos, Sym id);
      OpImp make_imp(Env &env);
      void dump_args(ostream &out) const override;
    };

    struct Nop: Op {
      static const OpType type;
      Nop(Env &env, Pos pos);
      OpImp make_imp(Env &env);
    };

    struct Push: Op {
      static const OpType type;     
      const Box val;
      Push(Env &env, Pos pos, const Box &val);
      
      template <typename ValT, typename... ArgsT>
      Push(Env &env, Pos pos, Type<ValT> &type, ArgsT &&...args):
        Op(Push::type, pos, make_imp(env)), val(type, forward<ArgsT>(args)...) { }

      OpImp make_imp(Env &env);
      void dump_args(ostream &out) const override;
    };

    struct Recall: Op {
      static const OpType type;
      Recall(Env &env, Pos pos);
      OpImp make_imp(Env &env);
    };

    struct Return: Op {
      static const OpType type;
      Return(Env &env, Pos pos);
      OpImp make_imp(Env &env);
    };

    struct Rot: Op {
      static const OpType type;
      Rot(Env &env, Pos pos);
      OpImp make_imp(Env &env);
    };

    struct RSwap: Op {
      static const OpType type;
      RSwap(Env &env, Pos pos);
      OpImp make_imp(Env &env);
    };

    struct Scope: Op {
      static const OpType type;
      Scope(Env &env, Pos pos);
      OpImp make_imp(Env &env);
    };

    struct ScopeEnd: Op {
      static const OpType type;
      ScopeEnd(Env &env, Pos pos);
      OpImp make_imp(Env &env);
    };

    struct SDrop: Op {
      static const OpType type;
      SDrop(Env &env, Pos pos);
      OpImp make_imp(Env &env);
    };

    struct Split: Op {
      static const OpType type;
      const Int offs;
      Split(Env &env, Pos pos, Int offs=0);
      OpImp make_imp(Env &env);
    };

    struct SplitEnd: Op {
      static const OpType type;
      SplitEnd(Env &env, Pos pos);
      OpImp make_imp(Env &env);
    };

    struct Stack: Op {
      static const OpType type;
      const bool end_split;
      Stack(Env &env, Pos pos, bool end_split);
      OpImp make_imp(Env &env);
    };

    struct Stop: Op {
      static const OpType type;
      Stop(Env &env, Pos pos);
      OpImp make_imp(Env &env);
    };

    struct Swap: Op {
      static const OpType type;
      Swap(Env &env, Pos pos);
      OpImp make_imp(Env &env);
    };

    struct Sync: Op {
      static const OpType type;
      Sync(Env &env, Pos pos);
      OpImp make_imp(Env &env);
    };

    struct Task: Op {
      static const OpType type;
      PC start_pc;
      Int end_pc;
      Task(Env &env, Pos pos);
      OpImp make_imp(Env &env);
    };

    struct Throw: Op {
      static const OpType type;
      Throw(Env &env, Pos pos);
      OpImp make_imp(Env &env);
    };
    
    struct Times: Op {
      static const OpType type;
      const Int i_reg;
      Times(Env &env, Pos pos, Int i_reg);
      OpImp make_imp(Env &env);
    };

    struct Try: Op {
      static const OpType type;
      const Int state_reg;
      Int end_pc;
      Try(Env &env, Pos pos, Int state_reg);
      OpImp make_imp(Env &env);
    };

    struct TryEnd: Op {
      static const OpType type;
      const Int state_reg;
      TryEnd(Env &env, Pos pos, Int state_reg);
      OpImp make_imp(Env &env);
    };

    struct Yield: Op {
      static const OpType type;
      Yield(Env &env, Pos pos);
      OpImp make_imp(Env &env);
    };
  } 
}

#endif
